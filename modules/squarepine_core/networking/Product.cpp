//==============================================================================
namespace product
{
static SharedResourcePointer<Array<Identifier>> compatibleIds;

inline void initialiseFilterIfNeeded()
{
    if (! compatibleIds->isEmpty())
        return;

    compatibleIds->operator=(
        { authorNameId,
          authorUuidId,
          imageUrlId,
          heroImageUrlId,
          numLikesId,
          commentsId,
          uuidId,
          skuId,
          slugId,
          titleId,
          descriptionId,
          permalinkId,
          dateCreatedId,
          dateModifiedId,
          priceId,
          regularPriceId,
          dateOnSaleFromId,
          dateOnSaleToId,
          totalSalesId,
          averageRatingId,
          ratingCountId,
          isPurchasableId,
          isFeaturedId,
          isOnSaleId,
          isVirtualId,
          isInStockId,
          isDownloadableId,
          areReviewsAllowedId,
          downloadableFilesId,
          downloadLimitId,
          downloadExpiryId,
          externalUrlId,
          taxStatusId,
          taxClassId,
          categoriesId,
          imagesId,
          attributesId,
          attributesId,
          virtualDevicesId,
          metadataId });
}

inline void populateSetWithFilter (ValueTree& state, const NamedValueSet::NamedValue& prop)
{
    initialiseFilterIfNeeded();

    if (! std::binary_search (compatibleIds->begin(), compatibleIds->end(), prop.name))
        state.setProperty (prop.name, prop.value, nullptr);
}

static StringArray createStringArray (const var& v)
{
    StringArray results;

    if (auto* object = v.getDynamicObject())
        for (const auto& prop: object->getProperties())
            results.add (prop.value.toString());

    if (auto* array = v.getArray())
        for (const auto& item: *array)
            results.add (item.toString());

    results.trim();
    results.removeEmptyStrings();
    results.removeDuplicates (true);
    results.minimiseStorageOverheads();

    return results;
}
}

//==============================================================================
namespace sorting
{
template<typename ReturnType, ReturnType (Product::*Function)() const>
struct ProductPropertyComparator
{
    static int compareElements (const Product& first, const Product& second) noexcept
    {
        const auto a = (first.*Function)();
        const auto b = (second.*Function)();

        if (a < b)
            return -1;
        if (a > b)
            return 1;

        return 0;
    }

    static int compareElements (const Product* first, const Product* second) noexcept
    {
        jassert (first != nullptr && second != nullptr);
        return compareElements (*first, *second);
    }
};

using ProductUuidComparator = ProductPropertyComparator<int64, &Product::getUuid>;
using ProductSkuComparator = ProductPropertyComparator<String, &Product::getSKU>;
using ProductAuthorComparator = ProductPropertyComparator<String, &Product::getAuthorName>;
using ProductAuthorUuidComparator = ProductPropertyComparator<int64, &Product::getAuthorUuid>;
using ProductPriceComparator = ProductPropertyComparator<double, &Product::getPrice>;
using ProductSalePriceComparator = ProductPropertyComparator<double, &Product::getSalePrice>;
}

//==============================================================================
Product::Product(): state (product::productId) {}

Product::Product (const var& s): state (product::productId)
{
    if (auto* object = s.getDynamicObject())
    {
        for (const auto& prop: object->getProperties())
            product::populateSetWithFilter (state, prop);
    }
    else
    {
        jassertfalse;
    }
}

Product::Product (const ValueTree& s): state (s)
{
    jassert (state.hasType (product::productId));
}

Product::Product (const Product& other): state (other.state)
{
}

Product::Product (Product&& other): state (other.state)
{
}

Product& Product::operator= (const Product& other)
{
    state = other.state;
    return *this;
}

Product& Product::operator= (Product&& other)
{
    state = std::move (other.state);
    return *this;
}

bool Product::operator== (const Product& other) const noexcept { return getUuid() == other.getUuid(); }
bool Product::operator!= (const Product& other) const noexcept { return ! operator== (other); }
bool Product::operator< (const Product& other) const noexcept { return getUuid() < other.getUuid(); }
bool Product::operator<= (const Product& other) const noexcept { return getUuid() <= other.getUuid(); }
bool Product::operator> (const Product& other) const noexcept { return getUuid() > other.getUuid(); }
bool Product::operator>= (const Product& other) const noexcept { return getUuid() >= other.getUuid(); }

void Product::updateFromNewer (const Product& product)
{
    if (getUuid() == product.getUuid()
        && getDateModified() < product.getDateModified())
    {
        operator= (product);
    }
}

//==============================================================================
String Product::getAuthorName() const
{
    return {};
}

int64 Product::getAuthorUuid() const
{
    return {};
}

int64 Product::getUuid() const { return getVarAsString (state, product::uuidId).getLargeIntValue(); }
int64 Product::getNumLikes() const { return getVarAsString (state, product::numLikesId).getLargeIntValue(); }

bool Product::isPurchasable() const { return getVarAsBool (state, product::isPurchasableId); }
bool Product::isFeatured() const { return getVarAsBool (state, product::isFeaturedId); }
bool Product::isOnSale() const { return getVarAsBool (state, product::isOnSaleId); }
bool Product::isVirtual() const { return getVarAsBool (state, product::isVirtualId); }
bool Product::isDownloadable() const { return getVarAsBool (state, product::isDownloadableId); }
bool Product::isInStock() const { return getVarAsBool (state, product::isInStockId); }
bool Product::areReviewsAllowed() const { return getVarAsBool (state, product::areReviewsAllowedId); }
String Product::getSKU() const { return getVarAsString (state, product::skuId); }
String Product::getSlug() const { return getVarAsString (state, product::slugId); }
String Product::getTitle() const { return getVarAsString (state, product::titleId); }
String Product::getDescription() const { return getVarAsString (state, product::descriptionId); }
String Product::getPermalink() const { return unescapeUrls (getVarAsString (state, product::permalinkId)); }
String Product::getImageUrl() const { return unescapeUrls (getVarAsString (state, product::imageUrlId)); }
String Product::getHeroImageUrl() const { return unescapeUrls (getVarAsString (state, product::heroImageUrlId)); }
Time Product::getDateCreated() const { return getVarAsISO8601 (state, product::dateCreatedId); }
Time Product::getDateModified() const { return getVarAsISO8601 (state, product::dateModifiedId); }

double Product::getPrice() const { return getVarAsString (state, product::priceId).getDoubleValue(); }
double Product::getRegularPrice() const { return getVarAsString (state, product::regularPriceId).getDoubleValue(); }
double Product::getSalePrice() const { return getVarAsString (state, product::salePriceId).getDoubleValue(); }

StringArray Product::getCategories() const { return product::createStringArray (state.getProperty ("categories")); }
StringArray Product::getAttributes() const { return product::createStringArray (state.getProperty ("attributes")); }
StringArray Product::getVirtualDevices() const { return product::createStringArray (state.getProperty ("virtual_devices")); }
StringArray Product::getComments() const { return product::createStringArray (state.getProperty ("reviews")); }

//==============================================================================
namespace ProductFinder
{
//==============================================================================
#undef RETURN_TYPE_WITH_NAME
#define RETURN_TYPE_WITH_NAME(enumClassName, varName) \
    case enumClassName::varName:                      \
        return JUCE_STRINGIFY (varName);

String toString (Context c)
{
    switch (c)
    {
        RETURN_TYPE_WITH_NAME (Context, edit)
        default:
            break;
    };

    return "view";
}

String toString (Status s)
{
    switch (s)
    {
        RETURN_TYPE_WITH_NAME (Status, draft)
        RETURN_TYPE_WITH_NAME (Status, pending)
        RETURN_TYPE_WITH_NAME (Status, publish)
        case Status::privatised:
            return "private";
        default:
            break;
    };

    return "any";
}

String toString (Type t)
{
    switch (t)
    {
        RETURN_TYPE_WITH_NAME (Type, grouped)
        RETURN_TYPE_WITH_NAME (Type, external)
        RETURN_TYPE_WITH_NAME (Type, variable)
        default:
            break;
    };

    return "simple";
}

String toString (SortOrder so)
{
    switch (so)
    {
        RETURN_TYPE_WITH_NAME (SortOrder, id)
        RETURN_TYPE_WITH_NAME (SortOrder, include)
        RETURN_TYPE_WITH_NAME (SortOrder, title)
        RETURN_TYPE_WITH_NAME (SortOrder, slug)
        default:
            break;
    };

    return "date";
}

String toString (TaxClass tc)
{
    switch (tc)
    {
        case TaxClass::reducedRate:
            return "reduced-rate";
        case TaxClass::zeroRate:
            return "zero-rate";
        default:
            break;
    };

    return "standard";
}

String toString (StockStatus ss)
{
    switch (ss)
    {
        case StockStatus::outOfStock:
            return "outofstock";
        case StockStatus::onBackOrder:
            return "onbackorder";
        default:
            break;
    };

    return "instock";
}

#undef RETURN_TYPE_WITH_NAME

//==============================================================================
Context createContextFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("edit"))
        return Context::edit;
    else if (r.equalsIgnoreCase ("view"))
        return Context::view;

    jassertfalse;
    return Context::view;
}

Status createStatusFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("draft"))
        return Status::draft;
    else if (r.equalsIgnoreCase ("pending"))
        return Status::pending;
    else if (r.equalsIgnoreCase ("privatised"))
        return Status::privatised;
    else if (r.equalsIgnoreCase ("publish"))
        return Status::publish;
    else if (r.equalsIgnoreCase ("any"))
        return Status::any;

    jassertfalse;
    return Status::any;
}

Type createTypeFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("grouped"))
        return Type::grouped;
    else if (r.equalsIgnoreCase ("external"))
        return Type::external;
    else if (r.equalsIgnoreCase ("variable"))
        return Type::variable;
    else if (r.equalsIgnoreCase ("simple"))
        return Type::simple;

    jassertfalse;
    return Type::simple;
}

SortOrder createSortOrderFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("id"))
        return SortOrder::id;
    else if (r.equalsIgnoreCase ("include"))
        return SortOrder::include;
    else if (r.equalsIgnoreCase ("title"))
        return SortOrder::title;
    else if (r.equalsIgnoreCase ("slug"))
        return SortOrder::slug;
    else if (r.equalsIgnoreCase ("date"))
        return SortOrder::date;

    jassertfalse;
    return SortOrder::date;
}

TaxClass createTaxClassFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("reduced-rate"))
        return TaxClass::reducedRate;
    else if (r.equalsIgnoreCase ("zero-rate"))
        return TaxClass::zeroRate;
    else if (r.equalsIgnoreCase ("standard"))
        return TaxClass::standard;

    jassertfalse;
    return TaxClass::standard;
}

StockStatus createStockStatusFromString (const String& s)
{
    const auto r = s.trim();
    jassert (r.isNotEmpty());

    if (r.equalsIgnoreCase ("outofstock"))
        return StockStatus::outOfStock;
    else if (r.equalsIgnoreCase ("backorder"))
        return StockStatus::onBackOrder;
    else if (r.equalsIgnoreCase ("instock"))
        return StockStatus::inStock;

    jassertfalse;
    return StockStatus::inStock;
}

//==============================================================================
void SearchParameters::addIfNotEmpty (StringPairArray& dest, StringRef paramName, String source) const
{
    source.trim();

    jassert (! dest.containsKey (paramName));

    if (paramName.isNotEmpty() && source.isNotEmpty())
        dest.set (paramName, source);
}

void SearchParameters::populate (StringPairArray& spa) const
{
    addIfNotEmpty (spa, "context", toString (context));
    addIfNotEmpty (spa, "orderby", toString (sortOrder));
    addIfNotEmpty (spa, "status", toString (status));
    addIfNotEmpty (spa, "type", toString (type));
    addIfNotEmpty (spa, "tax_class", toString (taxClass));
    addIfNotEmpty (spa, "stock_status", toString (stockStatus));

    if (before != nullptr)
        addIfNotEmpty (spa, "before", before->toISO8601 (true));
    if (after != nullptr)
        addIfNotEmpty (spa, "after", after->toISO8601 (true));

    addIfNotEmpty (spa, "search", mustContainKeyword);
    addIfNotEmpty (spa, "slug", mustContainSlug);
    addIfNotEmpty (spa, "sku", mustContainSku);
    addIfNotEmpty (spa, "category", categoryId);
    addIfNotEmpty (spa, "tag", tagId);
    addIfNotEmpty (spa, "shipping_class", shippingClassId);
    addIfNotEmpty (spa, "attribute", attribute);
    addIfNotEmpty (spa, "attribute_term", attributeTermId);
    addIfNotEmpty (spa, "min_price", minPrice);
    addIfNotEmpty (spa, "max_price", maxPrice);

    if (isAscending)
        addIfNotEmpty (spa, "order", "asc");//NB: default when unspecified is descending ("desc")
    if (mustBeFeatured)
        addIfNotEmpty (spa, "featured", "1");
    if (mustBeOnSale)
        addIfNotEmpty (spa, "on_sale", "1");

    if (pageIndex != 1)
        spa.set ("page", String (pageIndex));
    if (pageSize != 10)
        spa.set ("per_page", String (std::clamp (pageSize, 1, 1000)));
    if (offset > 0)
        spa.set ("offset", String (std::clamp (offset, 1, 1000)));

    spa.minimiseStorageOverheads();
}

int findProducts (WooCommerceAccessPoint& wc,
                  ReferenceCountedArray<Product>& dest,
                  const SearchParameters& sp)
{
    if (! NetworkConnectivityChecker().isConnectedToInternet())
    {
        Logger::writeToLog ("No internet connection present. Will not be able to find products.");
        return -1;
    }

    StringPairArray spa;
    sp.populate (spa);

    dest.clear();

#if SQUAREPINE_LOG_NETWORK_CALLS
    double elapsedTimeSec = 0.0;
    auto stm = std::make_unique<ScopedTimeMeasurement> (elapsedTimeSec);
#endif

    const auto url = wc.createDomainAccessURLWithPath (woocommerce::get::products);

    if (auto s = createEndpointStream (url, HTTPRequest::GET))
    {
        auto body = s->readEntireStreamAsString();

#if SQUAREPINE_LOG_NETWORK_CALLS
        stm.reset();
        Logger::writeToLog ("Product listing download time took " + String (elapsedTimeSec) + " seconds." + newLine);

        beautifyJSON (body);
        Logger::writeToLog (String ("Received result:") + newLine + body);
#endif

        const auto result = JSON::parse (body);
        if (result.isArray())
        {
            if (auto* ar = result.getArray())
            {
                int numUniquelyAddedProducts = 0;

                for (const auto& item: *ar)
                {
                    if (auto p = std::make_unique<Product> (item))
                    {
                        if (! updateIfExisting (dest, *p.get()))
                        {
                            sorting::ProductUuidComparator comparator;
                            dest.addSorted (comparator, p.release());
                            ++numUniquelyAddedProducts;
                        }
                    }
                }

                return numUniquelyAddedProducts;
            }
        }

        jassertfalse;//Not sure what was wrong with the JSON here...
    }

    return -1;
}
}
