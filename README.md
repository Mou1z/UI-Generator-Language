# UI Generator Language
A wrapper language which translates to C++ code and utilized GTK/Cairo to draw shapes on the screen. The language supports variables, events and keybinds, making the output fairly interactive.

### Example Script:
```
CREATE `Rectangle` (top = 50%, left = 50%, width = 25px, height = 25px, origin_x = 0.5, origin_y = 0.5, fill_color = #FF0000FF) as `r1`
CREATE `Rectangle` (top = 0px, left = 0px, width = 10px, height = 10px, origin_x = 0.5, origin_y = 0.5, fill_color = #000000FF) as `cursor`
CREATE `Line` (start_x = 0px, start_y = 0px, end_x = 0px, end_y = 0px, stroke_width = 1, stroke_color = #000000FF) as `l1`

VAR speed = 1
VAR startX = 50
VAR startY = 50

event OnMouseMove
	UPDATE `cursor` (top = {y} : px, left = {x} : px)
	UPDATE `l1` (start_x = {startX} : %, start_y = {startY} : %, end_x = {x} : px, end_y = {y} : px)

event OnMouseDown
	UPDATE `l1` (stroke_color = #FF0000FF, stroke_width = 2)

event OnMouseUp
	UPDATE `l1` (stroke_color = #000000FF, stroke_width = 1)


event OnKeyPress
	IF keyval == 49
		SET speed = 1
	IF keyval == 50
		SET speed = 2
	IF keyval == 51
		SET speed = 4
	IF keyval == 65361
		SET startX -= speed
		UPDATE `r1` (left = {startX} : %)
	IF keyval == 65362
		SET startY -= speed
		UPDATE `r1` (top = {startY} : %)
	IF keyval == 65363
		SET startX += speed
		UPDATE `r1` (left = {startX} : %)
	IF keyval == 65364
		SET startY += speed
		UPDATE `r1` (top = {startY} : %)
	UPDATE `l1` (start_x = {startX} : %, start_y = {startY} : %)
```
