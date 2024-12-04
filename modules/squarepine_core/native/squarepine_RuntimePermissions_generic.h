/** Class to handle app runtime permissions for certain functionality on Android. */
class RuntimePermissions final
{
public:
    //==============================================================================
    /** Function type of runtime permission request callbacks. */
    using Callback = std::function<void (bool)>;

    /** Call this method to request a runtime permission.

        @param permission  The permission string you want to request.

        @param callback    The callback to be called after the request has been granted
                           or denied; the argument passed will be true if the permission
                           has been granted and false otherwise.

        If no runtime request is required or possible to obtain the permission, the
        callback will be called immediately. The argument passed in will be true
        if the permission is granted or no permission is required on this platform,
        and false otherwise.

        If a runtime request is required to obtain the permission, the callback
        will be called asynchronously after the OS has granted or denied the requested
        permission (typically by displaying a dialog box to the user and waiting until
        the user has responded).
    */
    static void request (const String&, Callback);

    static void request (const StringArray&, Callback);

    //==============================================================================
    /** @returns true if the runtime request is required to obtain
        the permission on the current platform.
    */
    static bool isRequired (const String&);

    /** @returns whether the provided runtime requests are required
        to obtain the permission on the current platform.
    */
    static bool isRequired (const StringArray&);

    //==============================================================================
    /** @returns true if the app has been already granted this permission,
        either via a previous runtime request or otherwise, or no permission is necessary.

        @note This can be false even if isRequired() returns false.
        In this case, the permission cannot be obtained at all at runtime.
    */
    static bool isGranted (const String&);

    /** @returns true if the app has been already granted the provided permissions,
        either via a previous runtime request or otherwise,
        or no permissions provided are necessary.

        @note This can return false even if isRequired() returns false.
        In this case, the permissions cannot be obtained at all at runtime.
    */
    static bool isGranted (const StringArray&);

private:
    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (RuntimePermissions)
};
