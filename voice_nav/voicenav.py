import speech_recognition as sr
import google.generativeai as genai
import json
import re
import pyttsx3
import genai_integration as genai_integration
import os
import sys

# Suppress ALSA error messages
sys.stderr = open(os.devnull, 'w')

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(parent_dir, "motor_control"))
import pi_to_motor
sys.path.append(os.path.join(parent_dir, "Vision"))
#import turn_detection
import dash_cam
import turn_detection

detection_process = turn_detection.start_detection()

API_KEY = os.getenv("GEMINI_API_KEY")

if not API_KEY:
    raise ValueError("Gemini API key not found. Please set it as an environment variable.")

genai.configure(api_key=API_KEY)

tts_engine = pyttsx3.init()
tts_engine.setProperty('rate',150)

def recognize_speech():
    recognizer = sr.Recognizer()    
    with sr.Microphone() as source:
        recognizer.adjust_for_ambient_noise(source, duration=0.5)
        print("Listening...")
        while True:          
            try:
                audio = recognizer.listen(source, timeout=5)
                print("Processing...")
                text = recognizer.recognize_google(audio)
                print("Recognized Speech: ",text)
                return text.lower()
            except sr.WaitTimeoutError:
                print("⏳ Timeout: No speech detected, continuing to listen...")
                continue 
            except sr.UnknownValueError:
                print("Could not understand speech.")
                continue
            except sr.RequestError as e:
                print(f"API Error: {e}")
                return None

def process_command(user_input):
    genai_integration.generate(user_input)
    response = str(genai_integration.result)
    print("full response: ",response)

    # Default Values
    direction = ""
    distance = 0
    object = ""
    time = 0
    angle = 0
    speed = 0
    reply = ""
    query = ""
    action = ""

    dic_strings = re.findall(r'\{.*?\}',response,re.DOTALL)
    print("dic_strings: ",dic_strings)
    extracted_data = []
    # extracted_data = extract_json_objects(response) 

    for dic_str in dic_strings:
        json_str = dic_str
        try:
            parsed_dic = json.loads(json_str)
            extracted_data.append(parsed_dic)
        except json.JSONDecodeError as e:
            print("Error parsing JSON:", e) 
        print("list_dics: ",extracted_data)   

    if extracted_data:
        for dic_temp in extracted_data:
            if dic_temp is None:
                continue
            action = dic_temp.get("action")
            if action == "move":
                direction = dic_temp.get("direction")         # need to fix this or 0 part, that will create exception.
                distance = int(dic_temp.get("distance") or 0)
                time = int(dic_temp.get("time") or 0)
                angle = int(dic_temp.get("angle") or 0)
                speed = int(dic_temp.get("speed") or 0)
                object = dic_temp.get("object")
            elif action == "response":
                reply = dic_temp.get("content","")
            elif action == "query":
                query = dic_temp.get("query","")

    print("processed data: ",direction,distance,time,angle,speed,reply,query,object)
    return {
        "direction": direction,
        "distance": distance,
        "time": time,
        "angle": angle,
        "speed": speed,
        "response": reply,
        "query": query,
        "object": object
    }
    
    
def speak_text(text):
    if text: 
        print("Speaking: ",text)
        tts_engine.say(text)
        tts_engine.runAndWait()

def query(query):
    print("Query received",query)
     
if __name__  == "__main__":
    dash_cam.start_stream()
    while True: 
        user_input = recognize_speech()

        if user_input:
            command_data = process_command(user_input)
            if command_data.get("object"):
                turn_detection.update_object(command_data.get("object")) 
                if hasattr(turn_detection, "detect_object", None) and getattr(turn_detection.detect_object, "is_object_found", False):
                    speak_text(f"{command_data.get('object')} found. Moving towards the {command_data.get('object')} now.")
            if command_data.get("direction"):
                key_remove = ["response", "query","object"]
                command_set = {k: v for k, v in command_data.items() if k not in key_remove}
                pi_to_motor.send_to_esp(command_set)
            elif command_data.get("response"):
                speak_text(command_data.get("response"))
                print(command_data.get("response"))
            elif command_data.get("query"):
                query(command_data.get("query"))