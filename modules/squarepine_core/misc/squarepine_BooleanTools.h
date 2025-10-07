/** A handy tool for converting a boolean to a desired primitive type

    @returns For a state of "true"; the result will be a value of "1" of the desired type.
             For a state of "false"; the result will be a value of "0" of the desired type.
*/
template<typename Type>
inline Type booleanToType (bool state) noexcept
{
    return state ? static_cast<Type> (1)
                 : static_cast<Type> (0);
}

/** A handy tool for converting a value to that of a boolean

    @returns For a non-zero value, the result will be "true".
             Otherwise, the result will be "false".
*/
template<typename Type>
inline bool typeToBoolean (Type value) noexcept
{
    return value != static_cast<Type> (0);
}

/** A handy tool for converting a boolean to a string

    @param state
    @param useYesNo For a state of "true"; the result will be either "Yes" or "True".
                        For a state of "false"; the result will be either "No" or "False".
*/
inline String booleanToString (bool state, bool useYesNo = false)
{
    return state ? (useYesNo ? TRANS ("Yes") : TRANS ("True"))
                 : (useYesNo ? TRANS ("No") : TRANS ("False"));
}
