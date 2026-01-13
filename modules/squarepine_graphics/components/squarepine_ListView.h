//==============================================================================
// Basic layout specifications
//==============================================================================

enum class Orientation
{
    vertical,
    horizontal
};

enum class AlignmentMode
{
    leading,
    centred,
    trailing
};

//==============================================================================
// Model layer
//==============================================================================

class ListModel
{
public:
    virtual ~ListModel() = default;

    virtual int getNumItems() const = 0;

    // Fixed extent for minimal prototype
    virtual float getItemExtent() const noexcept { return 24.0f; }

    // Provide or recycle a component for the given index
    virtual Component *getItemComponent(int index) = 0;
};

//==============================================================================
// Layout engine layer (pure geometry)
//==============================================================================

class LayoutEngine
{
public:
    virtual ~LayoutEngine() = default;

    virtual void setViewportSize(juce::Rectangle<float> size) = 0;
    virtual void setNumItems(int count) = 0;
    virtual void setScrollOffset(float offsetY) = 0;

    struct ItemGeometry
    {
        int index = 0;
        juce::Rectangle<float> bounds;

        bool operator== (const ItemGeometry& other) const
        {
            return index == other.index && bounds == other.bounds;
        }

        bool operator!= (const ItemGeometry& other) const
        {
            return ! operator== (other);
        }
    };

    virtual Array<ItemGeometry> getVisibleItems() const = 0;
    virtual float getTotalContentHeight() const = 0;
};

class VerticalLayoutEngine : public LayoutEngine
{
public:
    VerticalLayoutEngine() = default;

    void setViewportSize(juce::Rectangle<float> size) override
    {
        viewportHeight = size.getHeight();
    }

    void setNumItems(int count) override
    {
        numItems = count;
    }

    void setScrollOffset(float offsetY) override
    {
        scrollOffset = offsetY;
    }

    Array<ItemGeometry> getVisibleItems() const override
    {
        if (numItems <= 0 || viewportHeight <= 0.0f)
            return {};

        Array<ItemGeometry> out;

        const auto itemExtent = fixedItemExtent; // from model later
        const auto first = std::max(0, (int)std::floor(scrollOffset / itemExtent));
        const auto last = std::min(numItems - 1,
                                   (int)std::ceil((scrollOffset + viewportHeight) / itemExtent));

        for (int i = first; i <= last; ++i)
        {
            const auto y = i * itemExtent - scrollOffset;
            out.add({i, {0.0f, y, viewportWidth, itemExtent}});
        }

        return out;
    }

    float getTotalContentHeight() const override
    {
        return numItems * fixedItemExtent;
    }

private:
    int numItems = 0;
    float viewportWidth = 0.0f,
          viewportHeight = 0.0f,
          scrollOffset = 0.0f,
          fixedItemExtent = 24.0f;
};

//==============================================================================
// Recycling pool (minimal prototype)
//==============================================================================

class RecyclingPool
{
public:
    Component *acquire(int index, ListModel &model)
    {
        if (pool.isEmpty())
            return model.getItemComponent(index);

        return pool.removeAndReturn(0);
    }

    void release(Component *c)
    {
        pool.add(c);
    }

private:
    juce::OwnedArray<Component> pool;
};

//==============================================================================
// Scrollable view (ties model + layout + recycling together)
//==============================================================================

//==============================================================================
// Scroll physics strategy (optional, minimal)
//==============================================================================

class ScrollPhysics
{
public:
    virtual ~ScrollPhysics() = default;

    virtual float apply(float currentOffset, float delta)
    {
        return currentOffset + delta;
    }
};

//==============================================================================
// ScrollView (concrete, compositional, SOLID)
//==============================================================================
class ScrollView : public Component
{
public:
    ScrollView() = default;
    ~ScrollView() override = default;

    //==============================================================
    // Composition: model + layout + physics
    //==============================================================

    void setModel(ListModel *newModel)
    {
        model = newModel;

        if (layoutEngine != nullptr)
            layoutEngine->setNumItems(model != nullptr ? model->getNumItems() : 0);

        activeComponents.clearQuick (true);
        activeIndices.clearQuick();
        visibleItems.clearQuick();

        updateLayout();
        updateComponents();
        updatePositions();
    }

    void setLayoutEngine(std::unique_ptr<LayoutEngine> engine)
    {
        layoutEngine = std::move(engine);

        if (layoutEngine != nullptr)
            layoutEngine->setViewportSize(getLocalBounds().toFloat());

        if (layoutEngine != nullptr && model != nullptr)
            layoutEngine->setNumItems(model->getNumItems());

        updateLayout();
        updateComponents();
        updatePositions();
    }

    void setScrollPhysics(std::unique_ptr<ScrollPhysics> physics)
    {
        scrollPhysics = std::move(physics);
    }

    //==============================================================
    // Scrolling
    //==============================================================

    void setScrollOffset(float offset)
    {
        scrollOffset = juce::jmax(0.0f, offset);

        if (layoutEngine != nullptr)
            layoutEngine->setScrollOffset(scrollOffset);

        updateLayout();
        updateComponents();
        updatePositions();
    }

    float getScrollOffset() const
    {
        return scrollOffset;
    }

    void scrollBy(float delta)
    {
        if (scrollPhysics != nullptr)
            setScrollOffset(scrollPhysics->apply(scrollOffset, delta));
        else
            setScrollOffset(scrollOffset + delta);
    }

    //==============================================================
    // JUCE events
    //==============================================================

    void resized() override
    {
        if (layoutEngine != nullptr)
            layoutEngine->setViewportSize(getLocalBounds().toFloat());

        updateLayout();
        updatePositions (true); // IMPORTANT: no component rebuild here
    }

    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &d) override
    {
        scrollBy(-d.deltaY * 40.0f);
    }

private:
    //==============================================================
    // Responsibility 1: compute geometry
    //==============================================================

    void updateLayout()
    {
        Array<LayoutEngine::ItemGeometry> newLayout;

        if (layoutEngine != nullptr)
            newLayout = layoutEngine->getVisibleItems();

        if (newLayout != visibleItems)
            updateComponents();
    }

    //==============================================================
    // Responsibility 2: acquire/release components
    //==============================================================

    void updateComponents()
    {
        if (model == nullptr || layoutEngine == nullptr)
            return;

        // Build a set of indices that should be visible
        Array<int> newIndices;
        for (const auto &g : visibleItems)
            newIndices.add(g.index);

        // Release components that are no longer visible
        for (int i = activeComponents.size(); --i >= 0;)
        {
            auto *comp = activeComponents[i];
            const int index = activeIndices[i];

            if (!newIndices.contains(index))
            {
                removeChildComponent(comp);
                pool.release(comp);
                activeComponents.remove(i);
                activeIndices.remove(i);
            }
        }

        // Acquire components for newly visible items
        for (const auto &g : visibleItems)
        {
            if (! activeIndices.contains(g.index))
            {
                if (auto *comp = pool.acquire(g.index, *model))
                {
                    addAndMakeVisible(comp);
                    activeComponents.add(comp);
                    activeIndices.add(g.index);
                }
            }
        }
    }

    //==============================================================
    // Responsibility 3: position existing components
    //==============================================================

    void updatePositions(bool fromResizingCallback = false)
    {
        for (int i = 0; i < activeComponents.size(); ++i)
        {
            const int index = activeIndices[i];

            for (const auto &g : visibleItems)
            {
                if (g.index == index)
                {
                    activeComponents[i]->setBounds(g.bounds.toNearestInt());
                    break;
                }
            }
        }

        if (! fromResizingCallback)
            repaint();
    }

    //==============================================================
    // Members
    //==============================================================

    ListModel *model = nullptr;
    std::unique_ptr<LayoutEngine> layoutEngine = std::make_unique<VerticalLayoutEngine>();
    std::unique_ptr<ScrollPhysics> scrollPhysics = std::make_unique<ScrollPhysics>();
    RecyclingPool pool;
    OwnedArray<Component> activeComponents;
    Array<int> activeIndices;
    Array<LayoutEngine::ItemGeometry> visibleItems;
    float scrollOffset = 0.0f;
};

//==============================================================================
// Inline factory helpers (composition, not inheritance)
//==============================================================================

inline std::unique_ptr<ScrollView> makeListView()
{
    return std::make_unique<ScrollView>();
}

#if 0
inline std::unique_ptr<ScrollView> makeGridView (GridSpec spec)
{
    auto view = std::make_unique<ScrollView>();

    auto engine = std::make_unique<GridLayoutEngine>();
    engine->setGridSpec(spec);

    view->setLayoutEngine(std::move(engine));
    return view;
}

inline std::unique_ptr<ScrollView> makePageView (PageSpec spec)
{
    auto view = std::make_unique<ScrollView>();

    auto engine = std::make_unique<PageLayoutEngine>();
    engine->setPageSpec(spec);

    view->setLayoutEngine(std::move(engine));
    return view;
}
#endif
