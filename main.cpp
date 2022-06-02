/*

> TODO
    - Image Class
    - Text Class

-> Long Term
    - Ability to set parents for elements.
    - Animations (this depends upon the previous thing)
    - Variables
    - Conditionals
    - Extra Elements
        > Button
        > Input Field
        > Dropdown (It will be a pain to make this one, I won't bother making this. Mentioning it anyways)
        >

*/

#include <iostream>
#include <string>
#include <vector>

// Drawing Libraries
#include <gtkmm.h>
#include <cairomm/cairomm.h>

using namespace std;

bool strfind (string inputString, string targetString) {

    int index = 0;

    for (int i = 0; i < inputString.length (); i++) {
        if (inputString[i] == targetString[index]) {
            if (index == targetString.length () - 1) {
                return true;
            } else {
                index++;
                continue;
            }
        } else {
            if (index != 0) {
                index = 0;
                continue;
            }
        }
    }

    return false;
}

string getNumericalValue (string inputString) {
    string number = "";
    for (int i = 0; i < inputString.length (); i++) {
        if (
            inputString[i]  >= '0' || 
            inputString[i] <=  '9' || 
            inputString[i] == '-'
        ) {
            number += inputString[i];
        }
    }
    return number;
}

float processMagnitude (string inputValue, float relativeMagnitude = 1) {
    if (strfind (inputValue, "%")) {
        float value = float (stoi (getNumericalValue (inputValue))) / 100;
        return value * relativeMagnitude;
    } else if (strfind (inputValue, "px")) {
        return stoi (getNumericalValue (inputValue));
    }
    return -1;
}

class windowData {

    public:

        static int width;
        static int height;

        static void updateWindowSizeData (int w, int h) {
            width = w;
            height = h;
        }

};

int windowData::width (0);
int windowData::height (0);

// The drawing area widget
// An instance of 'Canvas' is created in the mainWindow class as a member which in-turn is set as a child of the main window.
class Canvas : public Gtk::DrawingArea {
    public:
        Canvas () {

            set_draw_func(sigc::mem_fun(*this, &Canvas::onDraw));

            auto controllerMotion = Gtk::EventControllerMotion::create ();
            controllerMotion->signal_motion ().connect (sigc::mem_fun (*this, &Canvas::onMouseMove));
            add_controller (controllerMotion);

            auto controllerGesture = Gtk::GestureClick::create ();
            controllerGesture->signal_pressed ().connect (sigc::mem_fun (*this, &Canvas::onMouseDown));
            controllerGesture->signal_released ().connect (sigc::mem_fun (*this, &Canvas::onMouseUp));
            add_controller (controllerGesture);

        }
        virtual ~Canvas () { /* Empty deconstructor for some reason. This needs to be empty otherwise it doesn't comple ! Need to find out why but too lazy. */ }

    protected:
        void onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        // Events
        void onMouseMove (double x, double y); 
        void onMouseDown (int n_press, double x, double y);
        void onMouseUp (int n_press, double x, double y);
        
};

// The main window widget
class mainWindow : public Gtk::Window {
    public:

        mainWindow ();

    protected:
    
        Canvas canvas;

        // Events
        bool onKeyPress (guint keyval, guint keycode, Gdk::ModifierType state);
        void onKeyRelease (guint keyval, guint keycode, Gdk::ModifierType state);
};

mainWindow::mainWindow () {

    set_title ("C++ UI Generator Project");
    set_child (canvas);

    auto controllerKey = Gtk::EventControllerKey::create ();
    controllerKey->signal_key_pressed ().connect (sigc::mem_fun (*this, &mainWindow::onKeyPress), false);
    controllerKey->signal_key_released ().connect (sigc::mem_fun (*this, &mainWindow::onKeyRelease));
    add_controller (controllerKey);

}

// 32-Bit Color (#RRGGBB[AA] in HEX form)
class Color32 {

    private:

        float red, green, blue, alpha;

    public:

        Color32 () {
            this->red = 0;
            this->green = 0;
            this->blue = 0;
            this->alpha = 0;
        }

        Color32 (float red, float green, float blue, float alpha) {
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->alpha = alpha;
        }

        // Some fixes and changes.
        // The values should be between 0-1 not 0-255, so we need to 'normalize' the converted hex to something between 0-1 by using the percentage formula.
        // We normally want to set the alpha value to maximum if it's not mentioned so that the color is visible with maximum opacity, it is for convenience. Most of the systems follow this convention.
        // I got rid of the extra parameter since we won't be needing it in this case, it will only slow thing down (It was only convenient in that javascript code because of some specific reasons). 
        // The constructor checks the string size and determines if 'alpha' is included or not - if not then alpha is set to 1.0f by default.
        Color32 (string hex) {
            
            this->red = 
                (float (stoi (hex.substr(1,2), 0, 16)) / 255.0f);

            this->green = 
                (float (stoi (hex.substr(3,2), 0, 16)) / 255.0f);

            this->blue = 
                (float (stoi (hex.substr(5,2), 0, 16)) / 255.0f);

            this->alpha = 
                hex.size () == 9 ? 
                    (float (stoi (hex.substr(7,2), 0, 16)) / 255.0f) : (1.0f);

        }

        // To make them read-only.
        float r () { return red;   }
        float g () { return green; }
        float b () { return blue;  }
        float a () { return alpha; }

};


class FillData {

    private:

        // More members might be added.
        Color32 color;

        //location of image
        string fileName;

    public:

        FillData () { }

        FillData (Color32 color) {
            this->color = color;
            this->fileName = "";
        }

        FillData (string fileName) {
            this->fileName = fileName;
            this->color = Color32(0,0,0,0);
        }

        bool isNull () {
            return color.a () == 0.0;
        }

        //not in use at the moment
        bool isFileNull () {
            return fileName == "";
        }

        Color32 getColor () {
            return color;
        }

        string getFile () {
            return fileName;
        }

};

class StrokeData {

    private:
        
        int width;
        Color32 color;

    public:
        
        StrokeData () {
            this->width = 0;
        }

        StrokeData (int width, Color32 color) {
            this->width = width;
            this->color = color;
        }

        bool isNull () {
            return color.a () == 0.0;
        }

        int getWidth () {
            return width;
        }

        Color32 getColor () {
            return color;
        }

        void setWidth (int width) {
            this->width = width;
        }

        void setColor (Color32 color) {
            this->color = color;
        }

};

class Coordinate {
    
    private:
        string x, y;

    public:

        Coordinate () {
            this->x = "0px";
            this->y = "0px";
        }

        Coordinate (int x, int y) {
            this->x = to_string (x) + "px";
            this->y = to_string (y) + "px";
        }

        Coordinate (string x, string y) {
            this->x = x;
            this->y = y;
        }

        int getX () { 
            return processMagnitude (x, windowData::width);
        }

        int getY () { 
            return processMagnitude (y, windowData::height);
        }

        void setX (string x) {
            this->x = x;
        }

        void setY (string y) {
            this->y = y;
        }

};

class Dimensions {

    private:

        string width;
        string height;

    public:

        Dimensions () {
            this->width = "0px";
            this->height = "0px";
        }

        Dimensions (string width, string height) {
            this->width = width;
            this->height = height;
        }

        float getWidth () {
            return processMagnitude (width, windowData::width);
        }

        float getHeight () {
            return processMagnitude (height, windowData::height);
        }

        void setWidth (string width) {
            this->width = width;
        }

        void setHeight (string height) {
            this->height = height;
        }

};

class Line {

    protected:

        Coordinate start;
        Coordinate end;

        StrokeData stroke;

    public:

        Line (Coordinate start, Coordinate end, StrokeData stroke) {
            this->start = start;
            this->end = end;
            this->stroke = stroke;
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {
            if (!stroke.isNull ()) {

                context->move_to (start.getX (), start.getY ());
                context->line_to (end.getX (), end.getY ());

                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );

                context->set_line_width (stroke.getWidth ());

                context->stroke ();
            }
        }

        Coordinate getStart () {
            return start;
        }

        Coordinate getEnd () {
            return end;
        }

        void setStart (Coordinate newStart) {
            this->start = newStart;
        }

        void setEnd (Coordinate newEnd) {
            this->end = newEnd;
        }

        void set_start_x (string x) {
            this->start.setX (x);
        }

        void set_start_y (string y) {
            this->start.setY (y);
        }

        void set_end_x (string x) {
            this->end.setX (x);
        }

        void set_end_y (string y) {
            this->end.setY (y);
        }

        void set_stroke_width (int width) {
            this->stroke.setWidth (width);
        }

        void set_stroke_color (Color32 color) {
            this->stroke.setColor (color);
        }

};

// 

class Path {

    private:

        vector<Coordinate> points;
        StrokeData stroke;

    public:

        Path (vector<Coordinate> points, StrokeData stroke) {
            this->points = points;
            this->stroke = stroke;
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {

            context->move_to (points[0].getX (), points[0].getY ());

            for (int i = 1; i < points.size (); i++) {
                context->line_to (points[i].getX (), points[i].getY ());
            }

            context->set_source_rgba (
                stroke.getColor ().r (),
                stroke.getColor ().g (),
                stroke.getColor ().b (),
                stroke.getColor ().a ()
            );

            context->set_line_width (stroke.getWidth ());

            context->stroke ();

        }

        vector<Coordinate> getPoints () {
            return points;
        }

        void addPoint (Coordinate newPoint) {
            points.push_back (newPoint);
        }

        void set_stroke_width (int width) {
            this->stroke.setWidth (width);
        }

        void set_stroke_color (Color32 color) {
            this->stroke.setColor (color);
        }

        void set_point_x (int i, string x) {
            this->points[i].setX (x);
        }

        void set_point_y (int i, string y) {
            this->points[i].setY (y);
        }

};

class Shape { 

    private:

        int z_index;
        string type;

    public:

        static int indices;
        static vector<Shape*> CreatedShapes;

        Shape () {
            this->z_index = indices;
            cout << indices << endl;
            indices++;
        }

        Shape (int z_index, int change = 0) {
            this->z_index = z_index;
            indices += change;
        }

        int get_z_index () {
            return z_index;
        }

        void set_z_index (int z_index) {
            this->z_index = z_index;
        }

        virtual bool isPointInsideShape (int x, int y) {
            cout << "Shape: Empty Method Called => isPointInsideShape ()" << endl;
            return false;
        }

        virtual string getType () {
            return "None";
        }

        virtual void draw (const Cairo::RefPtr<Cairo::Context>& context) = 0;

};

int Shape::indices = 0;
vector<Shape*> Shape::CreatedShapes {};

class Rectangle : public Shape {

    protected:

        Coordinate position;
        Dimensions dimensions;

        float originX;
        float originY;

        StrokeData stroke;
        FillData fill;

    public:

        Rectangle (
            
            Coordinate position,
            Dimensions dimensions,

            float originX,
            float originY,

            StrokeData stroke,
            FillData fill

        ) {

            this->position = position;
            this->dimensions = dimensions;

            this->originX = originX;
            this->originY = originY;

            this->stroke = stroke;
            this->fill = fill;

            Shape::CreatedShapes.push_back (this);

        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {

            context->rectangle (

                position.getX () - (originX * dimensions.getWidth ()), 
                position.getY () - (originY * dimensions.getHeight ()), 

                dimensions.getWidth (), 
                dimensions.getHeight ()
            );            

            if (!fill.isNull ()) {
                context->set_source_rgba (
                    fill.getColor ().r (),
                    fill.getColor ().g (),
                    fill.getColor ().b (),
                    fill.getColor ().a ()
                );
                context->fill_preserve ();
            }

            if (!stroke.isNull ()) {
                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );
                context->set_line_width (stroke.getWidth ());
                context->stroke ();
            }
            
        }

        bool isPointInsideShape (int x, int y) {

            // Look at these two lines first if there's any problem in the function.
            float pivotX = (originX * dimensions.getWidth ());
            float pivotY = (originY * dimensions.getHeight ());

            if (
                (position.getX () - pivotX) <= x && x <= (position.getX () + dimensions.getWidth () - pivotX)
                    &&
                (position.getY () - pivotY) <= y && y <= (position.getY () + dimensions.getHeight () - pivotY) 
            ) {
                return true;
            }

            return false;
        }

        void setFillData (FillData fill) {
            this->fill = fill;
        }

        string getType () {
            return "Rectangle";
        }

        void set_position_x (string x) {
            this->position.setX (x);
        }

        void set_position_y (string y) {
            this->position.setY (y);
        }

        void set_width (string width) {
            this->dimensions.setWidth (width);
        }

        void set_height (string height) {
            this->dimensions.setHeight (height);
        }

        void set_origin_x (float originX) {
            this->originX = originX;
        }

        void set_origin_y (float originY) {
            this->originY = originY;
        }

        void set_stroke_width (int width) {
            this->stroke.setWidth (width);
        }

        void set_stroke_color (Color32 color) {
            this->stroke.setColor (color);
        }

        void set_fill_color (Color32 color) {
            this->fill = FillData (color);
        }

};

bool ccw (Coordinate a, Coordinate b, Coordinate c) {
    return ((c.getY () - a.getY ()) * (b.getX () - a.getX ())) > ((b.getY () - a.getY ()) * (c.getX () - a.getX ()));
}

bool intersect (Coordinate a, Coordinate b, Coordinate c, Coordinate d) {
    return ccw (a, c, d) != ccw (b, c, d) && ccw (a, b, c) != ccw (a, b, d);
}

class Polygon : public Shape {

    private:

        vector<Coordinate> vertices;

        StrokeData stroke;
        FillData fill;

    public:

        Polygon (vector<Coordinate> vertices, StrokeData stroke, FillData fill) {
            this->vertices = vertices;
            this->stroke = stroke;
            this->fill = fill;

            Shape::CreatedShapes.push_back (this);
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {

            context->move_to (vertices[0].getX (), vertices[0].getY ());

            for (int i = 1; i < vertices.size (); i++) {
                context->line_to (vertices[i].getX(), vertices[i].getY ());
            }

            context->line_to (vertices[0].getX (), vertices[0].getY ());

            context->stroke_preserve ();

            if (!fill.isNull ()) {
                context->set_source_rgba (
                    fill.getColor ().r (),
                    fill.getColor ().g (),
                    fill.getColor ().b (),
                    fill.getColor ().a ()
                );
                context->fill ();
            }

        }

        bool isPointInsideShape (int x, int y) {

            int intersection = 0;

            for (int i = 0; i < 3; i++) {
                if 
                (
                    intersect (Coordinate (0, 0), Coordinate (x, y), Coordinate (vertices[i].getX (), vertices[i].getY ()), Coordinate (vertices[(i + 1) % vertices.size ()].getX (), vertices[(i + 1) % vertices.size ()].getY ()))
                ) 
                { 
                    intersection++; 
                }
            }

            return intersection % 2 != 0;
        }

        void setFillData (FillData fill) {
            this->fill = fill;
        }

        string getType () {
            return "Polygon";
        }

        void set_x (int i, string x) {
            this->vertices[i].setX (x);
        }

        void set_y (int y, string y) {
            this->vertices[i].setY (y);
        }

        void set_stroke_width (int width) {
            this->stroke.setWidth (width);
        }

        void set_stroke_color (Color32 color) {
            this->stroke.setColor (color);
        }

        void set_fill_color (Color32 color) {
            this->fill = FillData (color);
        }

};

// 

class Arc {

    private:

        Coordinate position;

        float radius;

        float startAngle;
        float endAngle;

        StrokeData stroke;
        FillData fill;


    public:

        Arc (Coordinate position, float radius, float startAngle, float endAngle, StrokeData stroke, FillData fill) {
            this->position = position;

            this->radius = radius;

            this->startAngle = startAngle;
            this->endAngle = endAngle;

            this->stroke = stroke;
            this->fill = fill;
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {
        
            context->arc (position.getX (), position.getY (), radius, startAngle, endAngle);

            if (!fill.isNull ()) {
                context->set_source_rgba (
                    fill.getColor ().r (),
                    fill.getColor ().g (),
                    fill.getColor ().b (),
                    fill.getColor ().a ()
                );
                context->fill_preserve ();
            }

            if (!stroke.isNull ()) {
                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );
                context->set_line_width (stroke.getWidth ());
                context->stroke ();
            }

        }

        void set_position_x (string x) {
            this->position.setX (x);
        }

        void set_position_y (string y) {
            this->position.setY (y);
        }

        void set_radius (float radius) {
            this->radius = radius;
        }

        void set_angle_start (float startAngle) {
            this->startAngle = startAngle;
        }

        void set_angle_end (float endAngle) {
            this->endAngle = endAngle;
        }

        void set_stroke_width (int width) {
            this->stroke.setWidth (width);
        }

        void set_stroke_color (Color32 color) {
            this->stroke.setColor (color);
        }

        void set_fill_color (Color32 color) {
            this->fill = FillData (color);
        }

};

class Image : public Rectangle {

    private:
    //properties might be added
    //File name of the image will be stored in the FillData class

    public:

        Image (
            
            Coordinate position,
            Dimensions dimensions,

            float originX,
            float originY,

            StrokeData stroke,
            FillData fill

        ) : Rectangle (position, dimensions, originX, originY, stroke, fill) {
            
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {
            
            auto image = Gdk::Pixbuf::create_from_file (fill.getFile());

            Gdk::Cairo::set_source_pixbuf (
                context,
                image, 
                position.getX (), 
                position.getY ()
            );
            context->paint();
        
            if (!stroke.isNull ()) {
                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );
                context->set_line_width (stroke.getWidth ());
                context->stroke ();
            } 

        }
        
};

// {creationCode}

Rectangle r1 (Coordinate ("50%", "50%"), Dimensions ("50%", "50%"), 0.5f, 0.5f, StrokeData (2, Color32 ("#000000FF")), FillData (Color32 ("#FF0000FF")));
Path p1 ({ Coordinate ("50%", "50%"), Coordinate ("75%", "75%"), Coordinate ("25%", "75%"), Coordinate ("50%", "50%") }, StrokeData (2, Color32 ("#000000FF")));

void Canvas::onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    windowData::updateWindowSizeData (width, height);

    // {onDrawCode}
    r1.draw (cr);
    p1.draw (cr);

}

////////////////////////////////////////////////////////////////////////
// EVENTS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/*

Supported Events:

    OnShapeClick (Not Completed)
    OnKeyPress (keycode)
    OnKeyRelease (keycode)
    OnMouseMove (click_count, x, y)
    OnMouseClick (click_count, x, y)
    OnMouseDown (click_count, x, y)
    OnMouseUp (click_count, x, y)

*/

void onShapeClick (Canvas& canvas, Shape& shape) {
    // Notes for myself:
    // Figure out what the translated code will look like.
    // Is the cast necessary ? There are just 2 shapes. What about circles ? It's a special case of 'Arc'.
    // Plus Arc cannot be passed into this function as it's not derived from 'Shape'.
    // Think of a way to support click detection on circles aswell, if it's too tedius them forget it.
    /*
    cout << "Shape Clicked: " << shape.getType () << endl;
    if (shape.getType() == "Rectangle") {
        Rectangle * rectangle = static_cast<Rectangle*>(&shape);
        rectangle->setFillData (FillData (Color32 (0, 1, 0, 1)));
    } else if (shape.getType() == "Polygon") {
        Polygon * polygon = static_cast<Polygon*>(&shape);
        
    }
    */
    // {OnShapeClick}
}

bool mainWindow::onKeyPress (guint keyval, guint keycode, Gdk::ModifierType state) {
    // {OnKeyPress}
    return true;
}

void mainWindow::onKeyRelease (guint keyval, guint keycode, Gdk::ModifierType state) {
    // {OnKeyRelease}
}

void Canvas::onMouseMove (double x, double y) {
    // {OnMouseMove}
}

void onMouseClick (Canvas& canvas, int clicks, double x, double y) {

    // {onMouseClick}

    int max = -1;
    for (int i = 0; i < Shape::CreatedShapes.size (); i++) {
        if (Shape::CreatedShapes[i]->isPointInsideShape (x, y)) {
            if (max == -1) { 
                max = i; 
            } else {
                if (Shape::CreatedShapes[i]->get_z_index() > 
                    Shape::CreatedShapes[max]->get_z_index ()) {
                    max = i;
                }
            }
        }
    }
    if (max != -1) {
        onShapeClick (canvas, *Shape::CreatedShapes[max]);
    }
}

void Canvas::onMouseDown (int n_press, double x, double y) {
    onMouseClick (*this, n_press, x, y);
    // {OnMouseDown}
}

void Canvas::onMouseUp (int n_press, double x, double y) {
    // {OnMouseUp}
}

////////////////////////////////////////////////////////////////////////
// MAIN ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  auto app = Gtk::Application::create("org.gtkmm.example");
  return app->make_window_and_run<mainWindow>(argc, argv);
}