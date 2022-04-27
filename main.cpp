/*

> TODO

1. Make a constructor in Color32 that takes in a string value that represents a HEX color code (#RRGGBBAA), and converts it into RGBA, finally storing the values in the relevant members.

*/

#include <iostream>
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

// 32-Bit Color (#RRGGBBAA in HEX form)
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

        // To make them read-only.
        float r () { return red;   }
        float g () { return green; }
        float b () { return blue;  }
        float a () { return alpha; }

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

};

class FillData {

    private:

        // More members might be added.
        Color32 color;

    public:

        FillData () { }

        FillData (Color32 color) {
            this->color = color;
        }

        bool isNull () {
            return color.a () == 0.0;
        }

        Color32 getColor () {
            return color;
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

};

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

};

int Shape::indices = 0;
vector<Shape*> Shape::CreatedShapes {};

class Rectangle : public Shape {

    private:

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

                position.getX () - (originX * windowData::width), 
                position.getY () - (originY * windowData::height), 

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

            float pivotX = (originX * windowData::width);
            float pivotY = (originY * windowData::height);

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

};

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


            // Not sure if we should support fill in this class
            // we'll see in the end what works best, ok.
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

};

Rectangle r1 (Coordinate ("100px", "100px"), Dimensions ("30%", "30%"), 0, 0, StrokeData (1, Color32 (0, 0, 0, 1)), FillData (Color32 (1, 0, 0, 1)));
Polygon t1 ({Coordinate ("200px", "200px"), Coordinate ("200px", "400px"), Coordinate ("250px", "100px")}, StrokeData (5, Color32 (0, 0, 0, 1)), FillData (Color32 (1, 0, 0, 1)));

void Canvas::onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    windowData::updateWindowSizeData (width, height);

    // onDraw Stuff
    r1.draw (cr);
    t1.draw (cr);
}

////////////////////////////////////////////////////////////////////////
// EVENTS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void onShapeClick (Shape shape) {
    cout << "Shape Clicked: " << shape.getType () << endl;
}

bool mainWindow::onKeyPress (guint keyval, guint keycode, Gdk::ModifierType state) {
    // On Key Press Stuff
    return true;
}

void mainWindow::onKeyRelease (guint keyval, guint keycode, Gdk::ModifierType state) {
    // On Key Release Stuff
}

void Canvas::onMouseMove (double x, double y) {
    // On Mouse Move Stuff
    /*
    if (r1.isPointInsideShape (x, y)) {
        r1.setFillData (FillData (Color32 (0, 1, 0, 1)));
    } else {
        r1.setFillData (FillData (Color32 (1, 0, 0, 1)));
    }

    if (t1.isPointInsideShape (x, y)) {
        t1.setFillData (FillData (Color32 (0, 1, 0, 1)));
    } else {
        t1.setFillData (FillData (Color32 (1, 0, 0, 1)));
    }
    */
    queue_draw ();
}

void onMouseClick (Canvas& canvas, int clicks, double x, double y) {
    // On Mouse Click Stuff
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
        cout << Shape::CreatedShapes[max]->getType () << endl;
        //onShapeClick (*Shape::CreatedShapes[max]);
    }
}

void Canvas::onMouseDown (int n_press, double x, double y) {
    onMouseClick (*this, n_press, x, y);
    // On Mouse Down
}

void Canvas::onMouseUp (int n_press, double x, double y) {
    // On Mouse Up
}

////////////////////////////////////////////////////////////////////////
// MAIN ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  auto app = Gtk::Application::create("org.gtkmm.example");
  return app->make_window_and_run<mainWindow>(argc, argv);
}
