#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Angle.h"
#include "Point3D.h"
#include "Size3D.h"
#include "Color.h"

class Transformations
{
public:
    explicit Transformations();

    explicit Transformations(Point3D translation, Size3D scale, Angle rotation);

    Point3D& translation();
    void translation(Point3D value);

    Size3D&  scale();
    void scale(Size3D value);

    Angle&   rotation();
    void rotation(Angle value);
    
    Color& color();
    void color(Color value);
    
    bool isVisible();
    void isVisible(bool value);
		
private:
    Point3D _translation;
    Size3D  _scale;
    Angle   _rotation;
    bool _useMatrixTransform;
    bool _isVisible;
    Color _color;
};

#endif
