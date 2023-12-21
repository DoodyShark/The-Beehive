# The-Beehive
Embedded Systems Challenge 2023 at NYU. Gesture Recognition from 3-axis accelerometer data on Adafruit Circuit Playground Classic board using the Arduino framework.

## Desrciption 
This is a run down on how we record the 10 gestures in order to obtain a proper reading through our accelerometer. It is important to properly train each gesture in order to produce the most accurate data. Gesture 0 is figure 8, gesture 1 is a infinity, gesture 2 is M, gesture 3 is W, gesture 4 is a clockwise circle, gesture 5 is a punch, gesture 6 is a wipe, gesture 7 is a swim motion, gesture 8 is a whirlpool, and gesture 9 is a triangle. There are 2 trials recorded for each gesture (see `gesture.h`) when training so that the recognition of the gesture is very accurate. 

It is important to note that for each person performing a gesture, they just make sure the positioning of the microcontroller in their hand is consistent so that the _domain-time-warp_ algorithm works properly. For example, if one wants to collect data and perform for the punch gesture, it is important for this person to hold and motion the microcontroller consistently. If both training trials reflect this, a punch should be detected everytime the gesture is performed. 

## Cross-reference message list 
Below is a cross-reference message list for the four nodes (in order) in the demo video: 

| Node        | Gesture 0 [8] | Gesture 1 [infinity] | Gesture 2 [M] | Gesture 3 [W] | Gesture 4 [clockwise cirle] | Gesture 5 [punch] | Gesture 6 [wipe] | Gesture 7 [swim] | Gesture 8 [whirlpool] | Gesture 9 [triangle] |
| ----------- | ------------- | -------------------- | ------------- | ------------- | --------------------------- | ----------------- | ---------------- | ---------------- | --------------------- | -------------------- |
| 1 (Dhiyaa)  | river         | apple                | mountain      | clock         | dream                       | dolphin           | mystery          | book             | rainbow               | star                 |
| 2 (Yufei)   | dream         | mystery              | dolphin       | river         | book                        | star              | rainbow          | apple            | mountain              | clock                |
| 3 (Neil)    | apple         | mountain             | star          | rainbow       | dolphin                     | dream             | mystery          | book             | clock                 | river                |
| 4 (Shaayan) | mountain      | book                 | clock         | rainbow       | dream                       | apple             | dolphin          | star             | mystery               | river                |

 
