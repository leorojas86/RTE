#include "Transformations.h"

#define SEPARATOR '|'

Transformations::Transformations()
{
    _scale              = Size3D(1, 1, 1);
    _useMatrixTransform = false;
    _isVisible          = true;
}

Transformations::Transformations(Point3D translation, Size3D scale, Angle rotation)
{
    _translation        = translation;
    _scale              = scale;
    _rotation           = rotation;
    _useMatrixTransform = false;
    _isVisible          = true;
}

Point3D& Transformations::translation()          { return _translation; }
void Transformations::translation(Point3D value) { _translation = value; }

Size3D& Transformations::scale()          { return _scale; }
void Transformations::scale(Size3D value) { _scale = value; }

Angle& Transformations::rotation()            { return _rotation; }
void Transformations::rotation(Angle value)   { _rotation = value; }

Color& Transformations::color()           { return _color; }
void Transformations::color(Color value)  { _color = value; }

bool Transformations::isVisible()             { return _isVisible; }
void Transformations::isVisible(bool value)   { _isVisible = value; }