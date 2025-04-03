import pyttsx3
tts_engine = pyttsx3.init()
tts_engine.setProperty('rate',150)

def speak_text(text):
    if text: 
        print("Speaking: ",text)
        tts_engine.say(text)
        tts_engine.runAndWait()