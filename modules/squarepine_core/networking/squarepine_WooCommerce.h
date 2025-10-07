//==============================================================================
/** This is the default website WordPress access point. */
class WordPressAccessPoint : public OAuth2AccessPoint
{
public:
    /** Constructs a basic access point to WordPress. */
    WordPressAccessPoint (const String& clientId,
                          const String& clientSecret);

    /** @internal */
    String getOAuth2EndpointPath() const override;
    /** @internal */
    String getOAuth2EndpointRevokePath() const override;
    /** @internal */
    String getOAuth2EndpointMePath() const override;

private: 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WordPressAccessPoint)
};

//==============================================================================
/** This is the default website WooCommerce access point. */
class WooCommerceAccessPoint : public OAuth2AccessPoint
{
public:
    /** Constructs a basic access point to WooCommerce. */
    WooCommerceAccessPoint (const String& clientId,
                            const String& clientSecret);

    /** @internal */
    String getOAuth2EndpointPath() const override;
    /** @internal */
    String getOAuth2EndpointRevokePath() const override;
    /** @internal */
    String getOAuth2EndpointMePath() const override;
    /** */
    URL createDomainAccessURLWithPath (const String& path) const override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WooCommerceAccessPoint)
};

//==============================================================================
namespace woocommerce
{
    namespace get
    {
        const String coupons                        = "/coupons";
        const String customers                      = "/customers";
        const String orders                         = "/orders";
        const String products                       = "/products";
        const String productVariations              = products + "/22/variations";
        const String productAttributes              = products + "/attributes";
        const String productAttributeTerms          = productAttributes + "/2/terms";
        const String productCategories              = products + "/categories";
        const String productShippingClasses         = products + "/shipping_classes";
        const String productTags                    = products + "/tags";
        const String productReviews                 = products + "/reviews";
        const String reports                        = "/reports";
        const String taxes                          = "/taxes";
        const String taxClasses                     = taxes + "/classes";
        const String webhooks                       = "/webhooks";
        const String settings                       = "/settings";
        const String paymentGateways                = "/payment_gateways";
        const String shippingZones                  = "/shipping/zones";
        const String shippingLocations              = shippingZones + "/5/locations";
        const String shippingZoneMethods            = shippingZones + "/5/methods";
        const String shippingMethods                = "shipping_methods";
        const String systemStatus                   = "system_status";
        const String systemStatusTools              = systemStatus + "/tools";
        const String systemStatusClearTransients    = systemStatusTools + "/clear_transients";
        const String data                           = "/data";
        const String continents                     = data + "/continents";
        const String countries                      = data + "/countries";
        const String currencies                     = data + "/currencies";
    }
}
