namespace product
{
    /** This is the top-level identifier for ValueTree purposes only. */
    CREATE_INLINE_IDENTIFIER (product)

    /** The assortment of product only properties are found here.
        All of these MUST math the JSON properties outlined in the docs.
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#products
        @see https://woocommerce.github.io/woocommerce-rest-api-docs/#product-properties
    */
    IMPLEMENT_IDENTIFIER (authorName,           "author")
    IMPLEMENT_IDENTIFIER (authorUuid,           "authorUuid")       //This doesn't exist yet
    IMPLEMENT_IDENTIFIER (imageUrl,             "imageUrl")         //This doesn't exist yet
    IMPLEMENT_IDENTIFIER (heroImageUrl,         "heroImageUrl")     //This doesn't exist yet
    IMPLEMENT_IDENTIFIER (numLikes,             "num_likes")        //This doesn't exist yet
    IMPLEMENT_IDENTIFIER (comments,             "comments")         //This doesn't exist yet

    IMPLEMENT_IDENTIFIER (virtualDevices,       "virtual_devices") //Enigmatic one... thought I saw it before but apparently it's not there?

    IMPLEMENT_IDENTIFIER (uuid,                 "id")
    IMPLEMENT_IDENTIFIER (sku,                  "sku")
    IMPLEMENT_IDENTIFIER (slug,                 "slug")
    IMPLEMENT_IDENTIFIER (title,                "name")
    IMPLEMENT_IDENTIFIER (description,          "description")
    IMPLEMENT_IDENTIFIER (permalink,            "permalink")
    IMPLEMENT_IDENTIFIER (dateCreated,          "date_created")
    IMPLEMENT_IDENTIFIER (dateModified,         "date_modified")
    IMPLEMENT_IDENTIFIER (price,                "price")
    IMPLEMENT_IDENTIFIER (regularPrice,         "regular_price")
    IMPLEMENT_IDENTIFIER (salePrice,            "sale_price")
    IMPLEMENT_IDENTIFIER (dateOnSaleFrom,       "date_on_sale_from")
    IMPLEMENT_IDENTIFIER (dateOnSaleTo,         "date_on_sale_to")
    IMPLEMENT_IDENTIFIER (totalSales,           "total_sales")
    IMPLEMENT_IDENTIFIER (averageRating,        "average_rating")
    IMPLEMENT_IDENTIFIER (ratingCount,          "rating_count")
    IMPLEMENT_IDENTIFIER (isPurchasable,        "purchasable")
    IMPLEMENT_IDENTIFIER (isFeatured,           "featured")
    IMPLEMENT_IDENTIFIER (isOnSale,             "on_sale")
    IMPLEMENT_IDENTIFIER (isVirtual,            "virtual")
    IMPLEMENT_IDENTIFIER (isInStock,            "in_stock")
    IMPLEMENT_IDENTIFIER (isDownloadable,       "downloadable")
    IMPLEMENT_IDENTIFIER (areReviewsAllowed,    "reviews_allowed")
    IMPLEMENT_IDENTIFIER (downloadableFiles,    "downloads")
    IMPLEMENT_IDENTIFIER (downloadLimit,        "download_limit")
    IMPLEMENT_IDENTIFIER (downloadExpiry,       "download_expiry")
    IMPLEMENT_IDENTIFIER (externalUrl,          "external_url")
    IMPLEMENT_IDENTIFIER (taxStatus,            "tax_status")
    IMPLEMENT_IDENTIFIER (taxClass,             "tax_class")
    IMPLEMENT_IDENTIFIER (categories,           "categories")
    IMPLEMENT_IDENTIFIER (images,               "images")
    IMPLEMENT_IDENTIFIER (attributes,           "attributes")
    IMPLEMENT_IDENTIFIER (defaultAttributes,    "default_attributes")
    IMPLEMENT_IDENTIFIER (metadata,             "meta_data")
}

/** @see https://woocommerce.github.io/woocommerce-rest-api-docs/#product-reviews */
namespace review
{
    enum class Status
    {
        approved = 0,
        hold,
        spam,
        unspam,
        trash,
        untrash
    };

    inline String toString (Status status)
    {
        switch (status)
        {
            case Status::approved:  break;
            case Status::hold:      return "hold";
            case Status::spam:      return "spam";
            case Status::unspam:    return "unspam";
            case Status::trash:     return "trash";
            case Status::untrash:   return "untrash";
            default: jassertfalse;  break;
        };

        return "approved";
    }

    inline Status createStatusfromString (const String& status)
    {
        const auto s = status.trim();
        jassert (s.isNotEmpty());

        if (s.equalsIgnoreCase ("approved"))        return Status::approved;
        else if (s.equalsIgnoreCase ("hold"))       return Status::hold;
        else if (s.equalsIgnoreCase ("spam"))       return Status::spam;
        else if (s.equalsIgnoreCase ("unspam"))     return Status::unspam;
        else if (s.equalsIgnoreCase ("trash"))      return Status::trash;
        else if (s.equalsIgnoreCase ("untrash"))    return Status::untrash;

        jassertfalse;
        return Status::approved;
    }

    IMPLEMENT_IDENTIFIER (id,               "id")
    IMPLEMENT_IDENTIFIER (dateCreated,      "date_created")
    IMPLEMENT_IDENTIFIER (dateCreatedGMT,   "date_created_gmt")
    IMPLEMENT_IDENTIFIER (productId,        "product_id")
    IMPLEMENT_IDENTIFIER (status,           "status")
    IMPLEMENT_IDENTIFIER (reviewer,         "reviewer")
    IMPLEMENT_IDENTIFIER (reviewerEmail,    "reviewer_email")
    IMPLEMENT_IDENTIFIER (review,           "review")
    IMPLEMENT_IDENTIFIER (rating,           "rating")
    IMPLEMENT_IDENTIFIER (isVerified,       "verified")
}

//==============================================================================
class Product final : public ReferenceCountedObject
{
public:
    Product();
    Product (const var& existingState);
    Product (const ValueTree& existingState);
    Product (Product&& other);
    Product (const Product& other);
    ~Product() = default;

    //==============================================================================
    String getAuthorName() const;
    int64 getAuthorUuid() const;
    int64 getUuid() const;
    String getSKU() const;
    String getSlug() const;
    String getTitle() const;
    String getDescription() const;
    String getPermalink() const;
    String getImageUrl() const;
    String getHeroImageUrl() const;
    Time getDateCreated() const;
    Time getDateModified() const;
    double getPrice() const;
    double getRegularPrice() const;
    double getSalePrice() const;
    int64 getNumLikes() const;
    bool isPurchasable() const;
    bool isFeatured() const;
    bool isOnSale() const;
    bool isVirtual() const;
    bool isDownloadable() const;
    bool isInStock() const;
    bool areReviewsAllowed() const;
    StringArray getCategories() const;
    StringArray getAttributes() const;
    StringArray getVirtualDevices() const;
    StringArray getComments() const;

    //==============================================================================
    void updateFromNewer (const Product& product);

    //==============================================================================
    /** The class is reference-counted) so this is a handy pointer class for it. */
    using Ptr = ReferenceCountedObjectPtr<Product>;

    //==============================================================================
    Product& operator= (const Product& other);
    Product& operator= (Product&&);
    bool operator== (const Product& other) const noexcept;
    bool operator!= (const Product& other) const noexcept;
    bool operator< (const Product& other) const noexcept;
    bool operator<= (const Product& other) const noexcept;
    bool operator> (const Product& other) const noexcept;
    bool operator>= (const Product& other) const noexcept;

    //==============================================================================
    static void sortByUuid (Array<Product>& products);
    static void sortByUuid (OwnedArray<Product>& products);
    static void sortByUuid (ReferenceCountedArray<Product>& products);

    static void sortByFavourited (Array<Product>& products);
    static void sortByFavourited (OwnedArray<Product>& products);
    static void sortByFavourited (ReferenceCountedArray<Product>& products);

    static void sortBySale (Array<Product>& products);
    static void sortBySale (OwnedArray<Product>& products);
    static void sortBySale (ReferenceCountedArray<Product>& products);

    static void sortByFeatured (Array<Product>& products);
    static void sortByFeatured (OwnedArray<Product>& products);
    static void sortByFeatured (ReferenceCountedArray<Product>& products);

    //==============================================================================
    ValueTree state;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (Product)
};

//==============================================================================
namespace ProductFinder
{
    //==============================================================================
    /** @returns true if the product instance provided was found in the provided container and subsequently updated. */
    inline bool updateIfExisting (ReferenceCountedArray<Product>& container, const Product& product)
    {
        for (auto* p : container)
        {
            if (p->operator== (product))
            {
                p->updateFromNewer (product);
                return true;
            }
        }

        return false;
    }

    enum class Context
    {
        view,
        edit
    };

    enum class Status
    {
        any,
        draft,
        pending,
        privatised,
        publish
    };

    enum class Type
    {
        simple,
        grouped,
        external,
        variable
    };

    enum class SortOrder
    {
        date,
        id,
        include,
        title,
        slug
    };

    enum class TaxClass
    {
        standard,
        reducedRate,
        zeroRate
    };

    enum class StockStatus
    {
        inStock,
        outOfStock,
        onBackOrder
    };

    String toString (Context);
    String toString (Status);
    String toString (Type);
    String toString (SortOrder);
    String toString (TaxClass);
    String toString (StockStatus);

    Context createContextFromString (const String& s);
    Status createStatusFromString (const String& s);
    Type createTypeFromString (const String& s);
    SortOrder createSortOrderFromString (const String& s);
    TaxClass createTaxClassFromString (const String& s);
    StockStatus createStockStatusFromString (const String& s);

    struct SearchParameters
    {
        SearchParameters() = default;
        ~SearchParameters() = default;

        struct IdentifierFilter
        {
            Array<int> inclusions, exclusions;
        };

        Context context = Context::view;
        Status status = Status::any;
        Type type = Type::simple;
        TaxClass taxClass = TaxClass::standard;
        StockStatus stockStatus = StockStatus::inStock;
        SortOrder sortOrder = SortOrder::date;
        int pageIndex = 1, pageSize = 100, offset = 0;
        bool isAscending = false;
        bool mustBeFeatured = false;
        bool mustBeOnSale = false;
        Time* before = nullptr;
        Time* after = nullptr;

        IdentifierFilter ids, parentIds;

        String mustContainKeyword, mustContainSlug, mustContainSku,
               minPrice, maxPrice,
               categoryId, tagId, shippingClassId,
               attribute, attributeTermId;

        void populate (StringPairArray& destination) const;

    private:
        void addIfNotEmpty (StringPairArray& dest, StringRef paramName, String source) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SearchParameters)
    };

    int findProducts (ReferenceCountedArray<Product>& dest,
                      const SearchParameters& searchParameters = {});
}
