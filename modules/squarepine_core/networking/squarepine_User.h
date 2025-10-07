//==============================================================================
namespace user
{
    /** This is the top-level identifier for ValueTree purposes only. */
    CREATE_INLINE_IDENTIFIER (user)

    /** The assortment of user only properties are found here.
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#customer-properties
    */
    CREATE_INLINE_IDENTIFIER (id)
    CREATE_INLINE_IDENTIFIER (date_created)
    CREATE_INLINE_IDENTIFIER (date_created_gmt)
    CREATE_INLINE_IDENTIFIER (date_modified)
    CREATE_INLINE_IDENTIFIER (date_modified_gmt)
    CREATE_INLINE_IDENTIFIER (email)
    CREATE_INLINE_IDENTIFIER (first_name)
    CREATE_INLINE_IDENTIFIER (last_name)
    CREATE_INLINE_IDENTIFIER (role)
    CREATE_INLINE_IDENTIFIER (username)
    CREATE_INLINE_IDENTIFIER (password)
    CREATE_INLINE_IDENTIFIER (is_paying_customer)
    CREATE_INLINE_IDENTIFIER (avatar_url)

    /** Top-level "billing" properties, with all of its sub-properties.
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#customer-properties
    */
    CREATE_INLINE_IDENTIFIER (billing)      // Top-level property
    namespace billing
    {
        CREATE_INLINE_IDENTIFIER (first_name)
        CREATE_INLINE_IDENTIFIER (last_name)
        CREATE_INLINE_IDENTIFIER (company)
        CREATE_INLINE_IDENTIFIER (address_1)
        CREATE_INLINE_IDENTIFIER (address_2)
        CREATE_INLINE_IDENTIFIER (city)
        CREATE_INLINE_IDENTIFIER (state)
        CREATE_INLINE_IDENTIFIER (postcode)
        CREATE_INLINE_IDENTIFIER (country)
        CREATE_INLINE_IDENTIFIER (email)
        CREATE_INLINE_IDENTIFIER (phone)
    }

    /** Top-level "shipping" properties, with all of its sub-properties.
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#customer-shipping-properties
    */
    CREATE_INLINE_IDENTIFIER (shipping)     // Top-level property
    namespace shipping
    {
        CREATE_INLINE_IDENTIFIER (first_name)
        CREATE_INLINE_IDENTIFIER (last_name)
        CREATE_INLINE_IDENTIFIER (company)
        CREATE_INLINE_IDENTIFIER (address_1)
        CREATE_INLINE_IDENTIFIER (address_2)
        CREATE_INLINE_IDENTIFIER (city)
        CREATE_INLINE_IDENTIFIER (state)
        CREATE_INLINE_IDENTIFIER (postcode)
        CREATE_INLINE_IDENTIFIER (country)
    }

    /** Top-level "meta_data" properties, with all of its sub-properties.
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#customer-meta-data-properties
    */
    CREATE_INLINE_IDENTIFIER (meta_data)    // Top-level property
    namespace metadata
    {
        CREATE_INLINE_IDENTIFIER (id)
        CREATE_INLINE_IDENTIFIER (key)
        CREATE_INLINE_IDENTIFIER (value)
    }
}

//==============================================================================
class User final : public ReferenceCountedObject
{
public:
    User();
    User (const ValueTree& existingState);
    User (const User& other);
    ~User();

    //==============================================================================
    /** The class is reference-counted, so this is a handy pointer class for it. */
    using Ptr = ReferenceCountedObjectPtr<User>;

    //==============================================================================
    static User::Ptr login (OAuth2AccessPoint& accessPoint, const String& username, const String& password);
    static bool refreshUserToken (OAuth2AccessPoint& accessPoint, User::Ptr user);
    static bool logout (OAuth2AccessPoint& accessPoint, User::Ptr user);

    const OAuth2Token& getToken() const noexcept { return token; }

    //==============================================================================
    #undef CREATE_STRING_GET_METHOD
    #define CREATE_STRING_GET_METHOD(name, id) \
        String get##name() const { return state.getProperty (id).toString(); }

    CREATE_STRING_GET_METHOD (FirstName, user::first_nameId)
    CREATE_STRING_GET_METHOD (LastName, user::last_nameId)
    CREATE_STRING_GET_METHOD (DateCreated, user::date_createdId)
    CREATE_STRING_GET_METHOD (DateCreatedGMT, user::date_created_gmtId)
    CREATE_STRING_GET_METHOD (DateModified, user::date_modifiedId)
    CREATE_STRING_GET_METHOD (DateModifiedGMT, user::date_modified_gmtId)
    CREATE_STRING_GET_METHOD (Email, user::emailId)
    CREATE_STRING_GET_METHOD (Role, user::roleId)
    CREATE_STRING_GET_METHOD (Username, user::usernameId)
    CREATE_STRING_GET_METHOD (Password, user::passwordId)
    CREATE_STRING_GET_METHOD (AvatarURL, user::avatar_urlId)

    #undef CREATE_STRING_GET_METHOD

    //==============================================================================
    #undef CREATE_GET_METHOD
    #define CREATE_GET_METHOD(name, id, returnType) \
        returnType name() const { return static_cast<returnType> (state.getProperty (id)); }

    CREATE_GET_METHOD (getUUID, user::idId, int)
    CREATE_GET_METHOD (isPayingCustomer, user::is_paying_customerId, bool)

    #undef CREATE_GET_METHOD

    //==============================================================================
    StringArray getBilling() const { return {}; }
    StringArray getShipping() const { return {}; }
    StringArray getMetadata() const { return {}; }
    StringArray getProducts() const { return {}; }

    //==============================================================================
    User& operator= (const User& other);
    bool operator== (const User& other) const noexcept;
    bool operator== (const User::Ptr other) const noexcept;
    bool operator!= (const User& other) const noexcept;
    bool operator!= (const User::Ptr other) const noexcept;

private:
    //==============================================================================
    OAuth2Token token;
    ValueTree state;

    //==============================================================================
    JUCE_LEAK_DETECTOR (User)
};
