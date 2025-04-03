import base64
import os
from google import genai
from google.genai import types


def generate(prompt_user):
    global result
    result = ""
    client = genai.Client(
        api_key="AIzaSyCSwkJsymBPNMzt-Hj3BQr6XClq8_zKZSE",
    )

    model = "gemini-2.0-flash"
    contents = [
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""so ye im building a smart voice automated navigation wheelchair called Swheels. the user will talk to swheels like go straight, take the next turn, stop, take the next slightly right turn or hard right. they may specify angles. they me specify speed or distance like go straight for 10 meters. my goal is to enable natural language input from user instead of using rigid pre-programmed instructions. so if a user ask swheel to go straight for 10 meter, u should return an output in dictionary format like {action: move, direction: forward, distance: 10, time: None,angle:None,speed:None} the values are None because it isnt specified by the user. like wise and i want u to handle similar and all possible instrctions. and for each commands or instruction from user, you should reply, which will then be text to speeced. like for the above user input u can reply like moving forward for 10 meters or for a different scenarios turning left, or stopping motion. use your creativity and respond according to the situation. The reply u give should be in this format {action: response, content: \"your reply\"} this way my raspberrypi script can extract the data and send it to appropirate functions and code block for the smooth functioning of the smart wheelchair. note that, this device is intended for people with mobility issued due to old age, health issues or partial or complete paralysis. you may respond like a smart ai assistant for general user queries/commands/inputs like when the user ask for time or anything unrelated to the system. if for example the user ask for the system info like the speed of shweels or the current temperature of system or angle of inclination, u should generate a dictionary like {action: query, query: speed} etc."""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I understand. I will act as the natural language processing component for your Swheels project. I'll receive text commands from the user, parse them into structured dictionaries, and provide relevant responses in the specified dictionary format. Here's how I'll approach different scenarios:

**1. Movement Commands:**

*   **Input:** \"Go straight for 10 meters.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"forward\",
    \"distance\": 10,
    \"time\": None,
    \"angle\": None,
    \"speed\": None
    }
    ```
*   **Response:**
    ```python
    {
    \"action\": \"response\",
    \"content\": \"Moving forward for 10 meters.\"
    }
    ```

*   **Input:** \"Take the next turn left.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"left\",
    \"distance\": None,
    \"time\": None,
    \"angle\": 90,  # Assume a typical left turn
    \"speed\": None
    }
    ```
*   **Response:**
    ```python
    {
    \"action\": \"response\",
    \"content\": \"Turning left at the next turn.\"
    }
    ```

*   **Input:** \"Hard right.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"right\",
    \"distance\": None,
    \"time\": None,
    \"angle\": 90, # You may decide what hard right means
    \"speed\": None
    }
    ```

    ```python
    {
    \"action\": \"response\",
    \"content\": \"Turning hard right.\"
    }
    ```

*   **Input:** \"Slightly right turn.\" (Let's assume slightly right is 30 degrees)
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"right\",
    \"distance\": None,
    \"time\": None,
    \"angle\": 30,
    \"speed\": None
    }
    ```
*   **Response:**
    ```python
    {
    \"action\": \"response\",
    \"content\": \"Turning slightly right.\"
    }
    ```

*   **Input:** \"Turn 45 degrees to the left.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"left\",
    \"distance\": None,
    \"time\": None,
    \"angle\": 45,
    \"speed\": None
    }
    ```
*   **Response:**
    ```python
    {
    \"action\": \"response\",
    \"content\": \"Turning 45 degrees to the left.\"
    }
    ```

*   **Input:** \"Go straight.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"forward\",
    \"distance\": None,
    \"time\": None,
    \"angle\": None,
    \"speed\": None
    }
    ```
*   **Response:**
    ```python
    {
    \"action\": \"response\",
    \"content\": \"Moving forward.\"
    }
    ```

*   **Input:** \"Go faster.\"
*   **Output:**
    ```python
    {
    \"action\": \"move\",
    \"direction\": \"forward\",
    \"distance\": None,
    \"time\": None,
    \"angle\": None,
    \"speed\": \"faster\" # You'll have to interpret 'faster' in code. You may add a scale of speed like from 1 to 10 etc.
    }
    ```

    ```python
    {
    \"action\": \"response\",
    \"content\": \"Increasing speed.\"
    }
    ```

*   **Input:** \"Stop.\"
*   **Output:**
    ```python
    {
    \"action\": \"stop\",
    \"direction\": None,
    \"distance\": None,
    \"time\": None,
    \"angle\": None,
    \"speed\": None
    }
    ```

    ```python
    {
    \"action\": \"response\",
    \"content\": \"Stopping.\"
    }
    ```
*   **Input:** \"Move backward a bit.\"
*    ```python
    {
    \"action\": \"move\",
    \"direction\": \"backward\",
    \"distance\": \"bit\",
    \"time\": None,
    \"angle\": None,
    \"speed\": None
    }
    ```
     *  **Response:**
        ```python
        {
        \"action\": \"response\",
        \"content\": \"Moving backward slightly.\"
        }
        ```
*     **Input:** \"Move slowly\"
*      **Output:**
       ```python
        {
        \"action\": \"move\",
        \"direction\": \"forward\",
        \"distance\": None,
        \"time\": None,
        \"angle\": None,
        \"speed\": \"slow\"
        }
       ```

    *   **Response:**
       ```python
        {
        \"action\": \"response\",
        \"content\": \"Moving slowly.\"
        }
       ```
**2. System Queries:**

*   **Input:** \"What is the current speed?\"
*   **Output:**
    ```python
    {
    \"action\": \"query\",
    \"query\": \"speed\"
    }
    ```

    I'd wait for the system to provide the actual speed value and respond accordingly in the format like this example \"The current speed is 2 meters per second.\". I will respond in appropriate format as soon as raspberry pi push data into me.
    ```python
    {
    \"action\": \"response\",
    \"content\": \"The current speed is 2 meters per second.\"
    }
    ```

*   **Input:** \"What is the battery level?\"
*   **Output:**
    ```python
    {
    \"action\": \"query\",
    \"query\": \"battery_level\"
    }
    ```

*    Similarly I would wait to retrieve data from raspberypi and respond appropriately.
     ```python
    {
    \"action\": \"response\",
    \"content\": \"The battery level is 85%.\"
    }
    ```

*   **Input:** \"What's the temperature?\"
*   **Output:**
    ```python
    {
    \"action\": \"query\",
    \"query\": \"temperature\"
    }
    ```
     ```python
    {
    \"action\": \"response\",
    \"content\": \"The system temperature is 25 degrees Celsius.\"
    }
    ```

*   **Input:** \"What is the angle of inclination?\"
*   **Output:**
    ```python
    {
    \"action\": \"query\",
    \"query\": \"inclination\"
    }
    ```

       ```python
    {
    \"action\": \"response\",
    \"content\": \"The current angle of inclination is 5 degrees uphill.\"
    }
    ```

**3. General Queries/Commands:**

*   **Input:** \"What time is it?\"
*   **Output:**
    ```python
    {
    \"action\": \"general\",
    \"query\": \"time\"
    }
    ```
    {
        \"action\": \"response\",
        \"content\": \"The current time is 10:30 AM.\" # This would ideally be generated based on a time lookup, or external time call in RPi
    }

*   **Input:** \"How are you?\"
*   **Output:**
    ```python
    {
    \"action\": \"general\",
    \"query\": \"wellbeing\" # May be other thing or random AI message to respond based on user preference.
    }
    ```

     ```python
    {
    \"action\": \"response\",
    \"content\": \"I am doing well, thank you for asking.\"
    }
    ```

**4.  Error Handling and Ambiguity:**

*   If the input is unclear, I'll ask for clarification:
    *   **Input:** \"Turn...\"
    *   **Output:**
        ```python
        {
        \"action\": \"error\",
        \"error_type\": \"incomplete_command\",
        \"details\": \"Please specify a direction (left or right) and optionally an angle or location.\"
        }
        ```

        ```python
        {
        \"action\": \"response\",
        \"content\": \"In which direction do you want to turn? Please specify left or right, and optionally an angle.\"
        }
        ```
*   If a distance is required and missing:
    *   **Input:** \"Go forward\"
      *    Output
      ```python
         {
         \"action\": \"move\",
         \"direction\": \"forward\",
         \"distance\": None,
         \"time\": None,
         \"angle\": None,
         \"speed\": None
        }
       ```

*   **Handling Numbers & Units** If you want to give instructions like go to a specific distance for more convenience I will process this cases too

    * Input : Go 5 steps front

    * Output:
    ```python
      {
         \"action\": \"move\",
         \"direction\": \"forward\",
         \"distance\": 5, #convert steps into distance assuming each step has the length of 1 m (for ex). u may define by giving steps like 5 * (length of a step which could be around 30cm)= 1.5
         \"time\": None,
         \"angle\": None,
         \"speed\": None
        }
    ```
     ```python
        {
        \"action\": \"response\",
        \"content\": \"Moving forward for 1.5m.\"
        }
       ```
    **Key Considerations:**

*   **Direction Mapping:**  You'll need to have a clear mapping in your code between \"left,\" \"right,\" \"forward,\" \"backward\" and how they relate to your wheelchair's motor controls.  Angle mappings also need to be precise for accurate turns.  You may also include options to process angle relative to direction or orientation of shweels if needed.
*   **Distance Measurement:** You need a reliable way to measure distance traveled, which may depend on wheel rotations or sensor data. A system to interpret relative instructions \"a bit,\" \"a short distance,\" \"far away\" is also necessary. This might require calibration.
*   **Speed Control:** Implement a speed control mechanism and mapping between natural language descriptors (faster, slower, moderate) and actual speed settings. Again, numeric value or scaling may work in favor.
*   **Safety:**  Include safety checks to prevent collisions (obstacle detection) and excessive speeds, especially when turning. Have Emergency protocols in your codes that allows for any interruptible/emergency actions during manual/autonomous navigation for user safety.
*   **Context:** You might want to maintain context. For example, if the user says \"Go straight for 5 meters,\" and then \"Turn left,\" you could assume the turn is relative to the new direction.  This can make the interaction more natural.
*   **Customization:** Think about the specific needs of users with mobility issues.  Consider adding features like memorizing frequent locations or customizable voice commands.

**How I'll learn & Improve:**

Ideally, with ongoing usage, you'd have a feedback loop. If the user frequently has to correct a particular instruction or the \"hard right\" is mis-interpreted, data can be logged and fed into me. The more training data (successful actions, corrections), the more accurate and adaptive I become.  It can even store common turns or personalized language patterns over time.
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""also no need for any explanations other than the required outputs which i already mentioned
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, understood. I will only provide the dictionary outputs and responses you requested, no additional explanations.
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""even though i aksed for dictionary like output, please refrain from using ``` python(that is the python formatitng in your generated output). this interferes with my code . ill convert the text like dictionary to proper dictionary and extract the data."""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Understood. I will avoid using the ```python formatting.
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""```json
{
\"action\": \"move\",
\"direction\": \"forward\",
\"distance\": 5,
\"time\": null,
\"angle\": null,
\"speed\": null
}
``` now the generated output is like. i only want plain text 



```json
{
\"action\": \"response\",
\"content\": \"Moving forward for 5 meters.\"
}
```"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I understand. No more code formatting. Plain text output only.
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""give me a sample output to check if its correct now

"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Input: Turn right

Output:
```
{
\"action\": \"move\",
\"direction\": \"right\",
\"distance\": null,
\"time\": null,
\"angle\": 90,
\"speed\": null
}
```

Response:
```
{
\"action\": \"response\",
\"content\": \"Turning right.\"
}
```
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""```json
{
\"action\": \"move\",
\"direction\": \"forward\",
\"distance\": 5,
\"time\": null,
\"angle\": null,
\"speed\": null
}
```

```json
{
\"action\": \"response\",
\"content\": \"Moving forward for 5 meters.\"
}
``` y am i getting json formatting now
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Apologies! Trying again.

Input: Turn right

Output:
```
{
\"action\": \"move\",
\"direction\": \"right\",
\"distance\": null,
\"time\": null,
\"angle\": 90,
\"speed\": null
}
```

Response:
```
{
\"action\": \"response\",
\"content\": \"Turning right.\"
}
```
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""its compulsory that every part of the output must be plain text without any formatting. i repeat every single part of the your output"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, understood. Plain text *only*.

Input: Turn right

Output:
```
{
\"action\": \"move\",
\"direction\": \"right\",
\"distance\": null,
\"time\": null,
\"angle\": 90,
\"speed\": null
}
```

Response:
```
{
\"action\": \"response\",
\"content\": \"Turning right.\"
}
```
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""generarte a sample output
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Input: Go backwards a little.

Output:
```
{
\"action\": \"move\",
\"direction\": \"backward\",
\"distance\": \"little\",
\"time\": null,
\"angle\": null,
\"speed\": null
}
```

Response:
```
{
\"action\": \"response\",
\"content\": \"Moving backwards a little.\"
}
```
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""apparently ``` are still applied to the output
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""You are absolutely right. My apologies. Let's try this again. I will remove the ticks now.

Input: Go backwards a little.

Output:
{
\"action\": \"move\",
\"direction\": \"backward\",
\"distance\": \"little\",
\"time\": null,
\"angle\": null,
\"speed\": null
}

Response:
{
\"action\": \"response\",
\"content\": \"Moving backwards a little.\"
}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""now can you return the output as a single line like this {\"action\": \"move\",\"direction\": \"forward\",\"distance\": 5,\"time\": null,\"angle\": null,\"speed\": null}"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I can provide output on a single line.

Input: Go backwards a little.

Output:
{\"action\": \"move\", \"direction\": \"backward\", \"distance\": \"little\", \"time\": null, \"angle\": null, \"speed\": null}

Response:
{\"action\": \"response\", \"content\": \"Moving backwards a little.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""the unit of speed should always be in kilometer per second, if user says in miles per hour , convert it to kilometer per hour while the response to the user being in terms of miles. as for user input like faster, slower, fast or slow, use these codes in place of speed, so if user input for speed instead of h/s value, then faster or fast  => 10005 or slower or slow = 10000."""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I understand the speed conversions and codes.

Input: Go faster.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": 10005}

Response:
{\"action\": \"response\", \"content\": \"Increasing speed.\"}

Input: Go at 60 miles per hour.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": 96.56}

Response:
{\"action\": \"response\", \"content\": \"Moving at 60 miles per hour.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""what if the user says move back a little or move forward a little, then the output distance will be 1 meter likewise adjust the distance for such scenarios"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I will set \"a little\" to 1 meter for distance when the user says \"move back a little\" or \"move forward a little.\"

Input: Move back a little.

Output:
{\"action\": \"move\", \"direction\": \"backward\", \"distance\": 1, \"time\": null, \"angle\": null, \"speed\": null}

Response:
{\"action\": \"response\", \"content\": \"Moving backward 1 meter.\"}

Input: Move forward a little.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": 1, \"time\": null, \"angle\": null, \"speed\": null}

Response:
{\"action\": \"response\", \"content\": \"Moving forward 1 meter.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""what id i ask to stop, then speed should be zero with action being move, since its related to movement"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I will make \"stop\" set the speed to zero with the action \"move.\"

Input: Stop.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": 0}

Response:
{\"action\": \"response\", \"content\": \"Stopping.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""hey provide a sample output for move forward for 5 meters"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Input: Move forward for 5 meters

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": 5, \"time\": null, \"angle\": null, \"speed\": null}

Response:
{\"action\": \"response\", \"content\": \"Moving forward for 5 meters.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""so among the output i want you to another element, so basically, if the user says something like, go straight and take the next door or go to the door, there should be another key called object which is initially null which later be updated with the object, here door or windows  or path etc
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I can add an \"object\" key to the output.

Input: Go straight and take the next door.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": null, \"object\": \"door\"}

Response:
{\"action\": \"response\", \"content\": \"Moving forward to the door.\"}

Input: Go straight and take the next window.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": null, \"object\": \"window\"}

Response:
{\"action\": \"response\", \"content\": \"Moving forward to the window.\"}

Input: Move forward.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": null, \"object\": null}

Response:
{\"action\": \"response\", \"content\": \"Moving forward.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""so if the object is mentioned or given by the user then distance and speed are not mandatory."""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Okay, I understand. If an object is specified, distance and speed are not mandatory.

Input: Go to the door.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": null, \"object\": \"door\"}

Response:
{\"action\": \"response\", \"content\": \"Moving towards the door.\"}

Input: Take the next turn towards the window.

Output:
{\"action\": \"move\", \"direction\": null, \"distance\": null, \"time\": null, \"angle\": null, \"speed\": null, \"object\": \"window\"}

Response:
{\"action\": \"response\", \"content\": \"Taking the next turn towards the window.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""make distance null if not mentioned by the user
"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Yes, distance will be null if not specified. All examples previously adhere to this principle as well.

Input: Turn Right to the window

Output:
{\"action\": \"move\", \"direction\": \"right\", \"distance\": null, \"time\": null, \"angle\": 90, \"speed\": null, \"object\": \"window\"}

Response:
{\"action\": \"response\", \"content\": \"Turning right towards the window.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""if no speed is mention, use 2kmph as defualt"""),
            ],
        ),
        types.Content(
            role="model",
            parts=[
                types.Part.from_text(text="""Understood. If no speed is mentioned, default to 3 kmph.

Input: Move forward to the door.

Output:
{\"action\": \"move\", \"direction\": \"forward\", \"distance\": null, \"time\": null, \"angle\": null, \"speed\": 3, \"object\": \"door\"}

Response:
{\"action\": \"response\", \"content\": \"Moving forward at 3 kilometers per hour to the door.\"}

Input: Turn left

Output:
{\"action\": \"move\", \"direction\": \"left\", \"distance\": null, \"time\": null, \"angle\": 90, \"speed\": 3, \"object\": null}

Response:
{\"action\": \"response\", \"content\": \"Turning left at 3 kilometers per hour.\"}
"""),
            ],
        ),
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text=prompt_user),
            ],
        ),
    ]
    generate_content_config = types.GenerateContentConfig(
        temperature=1,
        top_p=0.95,
        top_k=40,
        max_output_tokens=8192,
        response_mime_type="text/plain",
    )

    for chunk in client.models.generate_content_stream(
        model=model,
        contents=contents,
        config=generate_content_config,
    ):
        print(chunk.text, end="")
        result += chunk.text