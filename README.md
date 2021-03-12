# Menu Parseroo

## Goal

Create a semi-automated application for parsing menus, ready to digitise them.

### Why not fully automated?

The accuracy of the results is the most important thing, and the process cannot be fully automated without losing accuracy.

## Motivation
I spent some time doing data entry; specifically digitising menus for restaurants, takeaways, etc.
It is not a difficult task but is extremely boring, so naturally my mind went to automation.

## Analysis
Here is a general ruleset that most menus follow (I have seen some horrendous menus that have no clear categories):
- Menu is split into categories
- Each category contain a name and a list of items
- Each item has a name (left), atleast one price (right), and possibly a description (underneath)
- Categories may also include a list of sizes above item prices

Font rules:
- Category name is often a different colour to other text
- Category name uses the largest font size (excluding title)
- Item name and price uses the same font size
- Item description uses the smallest font size

### Proof of concept
I did some experimenting to see how viable of a project this would be.

I used EasyOCR with Python and found it to give fairly accurate text.
When identifying what each text represented I found that the text bounds were very inconsistent making it very difficult.
To solve this I learnt about font metrics.
Using the OCR results and image processing (OpenCV) I calculated the font metrics for each line of text, which gave me much more accurate text bounds.
From there it was quite easy to identify what each text represented.

At that point I was convinced that the project was very viable, but could not be fully automated with reliable results.

## Plan
Use C++ with:
- Qt for GUI
- OpenCV for image processing
- Tesseract OCR for OCR

User loads an image file of a menu.
Some preprocessing needs to be done to the image to prepare it for OCR.
Ideally want a binary image with black text and white background.
Can use OpenCV to convert image to grayscale and threshold it.
May want some other forms of image processing too.

Tesseract has an optimal character height so will likely need to scale image (https://groups.google.com/g/tesseract-ocr/c/Wdh_JJwnw94/m/24JHDYQbBQAJ).
We have already identified that menus use varying font sizes depending on what the text represents so this could cause an issue. May need to perform OCR on multiple scales and combine the results.

The OCR results will not always be correct so we need a stage where the user reviews the results and corrects any errors.

Once we have the text and it's size we can use the [font rules](#analysis) and Document Layout Analysis to identify what the text represents and the menu structure. From here we sort the text into a data structure which can then be integrated into digital menu systems.

### OCR Problems + Solutions
#### 1. False Positives
Sometimes an area of the image which does not contain text will be recognised by the OCR as text. These false positives would really mess with the next stage (Document Layout Analysis).

Possible solutions:
- The user can delete the false positives when reviewing the OCR results.
- During pre-processing we could add some simple paint tools so the user can remove parts of the image.

#### 2. False Negatives
Text that is not recognised by the OCR at all. The main cause of this seems to be text that is larger than the optimal character height, but it could also be caused by fancy fonts.

Possible solutions:
- Perform OCR at multiple scales and combine the results.
- When reviewing the OCR results the user could manually annotate any false negatives.

#### 3. Incorrect Recognition
Text that is recognised incorrectly by the OCR.

Possible solutions:
- Tesseract can be given a dictionary (I think it already has one? But it doesn't seem to enforce it only guide).
- The user can correct any incorrect recognition.

In reality it would be best if all of these solutions are implemented.
