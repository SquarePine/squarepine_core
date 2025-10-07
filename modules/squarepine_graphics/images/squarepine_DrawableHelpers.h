/** @returns a drawable parsed from the given file, assuming it was a valid SVG.
    A nullptr will be returned if there was a failure of any kind.
*/
std::unique_ptr<Drawable> createDrawableFromSVG (const File&);

/** @returns a drawable parsed from the given data, assuming it was a valid SVG.
    A nullptr will be returned if there was a failure of any kind.
*/
std::unique_ptr<Drawable> createDrawableFromSVG (const char* const data);

/** @returns an image created from the given Drawable, assuming it and the dimensions are valid.
    A null image will be returned if there was a failure of any kind.
*/
Image createImageFromDrawable (Drawable*, int width, int height);

/** Applies the given colour modifying function to each possible Drawable colour.

    This is applied recursively through all children.
*/
void applyToDrawableColours (Component&, std::function<void (Colour&)> func);

/** Replaces all colours with the given colour in the given Drawable.

    This is applied recursively through all children.
*/
void replaceAllDrawableColours (Component&, Colour colour);

/** Replaces every instance of the given source colour with the given dest
    colour in the provided Drawable.

    This is applied recursively through all children.
*/
void replaceAllDrawableColours (Component&, Colour source, Colour dest);
