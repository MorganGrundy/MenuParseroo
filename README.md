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
