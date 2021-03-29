/**
    Speaker X,Y Coordinates
         _____
      __| SPK |__
     /           \
    /_____________\
    1,0  _______ 0,0
        |       |
        |   *   |
        |_______|
    1,1          0,1
*/
struct PositionedImpulseResponse final
{
    /** */
    PositionedImpulseResponse (const AudioBuffer<float>& buffer, Point<float> pos) :
        impulseResponse (buffer),
        position (pos)
    {
    }

    /** */
    PositionedImpulseResponse (const AudioBuffer<float>& buffer, float x, float y) :
        PositionedImpulseResponse (buffer, { x, y })
    {
    }

    /** */
    PositionedImpulseResponse (const PositionedImpulseResponse& other) :
        PositionedImpulseResponse (other.impulseResponse, other.position)
    {
    }

    /** */
    PositionedImpulseResponse (PositionedImpulseResponse&& other) noexcept :
        impulseResponse (std::move (other.impulseResponse)),
        position (other.position)
    {
    }

    /** */
    PositionedImpulseResponse& operator= (PositionedImpulseResponse& other)
    {
        if (this != &other)
        {
            impulseResponse = other.impulseResponse;
            position = other.position;
        }

        return *this;
    }

    /** */
    PositionedImpulseResponse& operator= (PositionedImpulseResponse&& other) noexcept
    {
        impulseResponse = std::move (other.impulseResponse);
        position = other.position;
        return *this;
    }

    /** */
    float distanceFrom (float x, float y) const
    {
        x = std::pow (x - position.x, 2.0f);
        y = std::pow (y - position.y, 2.0f);
        return std::sqrt (x + y);
    }

    /** */
    float distanceFrom (Point<float> pos) const
    {
        return distanceFrom (pos.x, pos.y);
    }

    AudioBuffer<float> impulseResponse;
    Point<float> position;

private:
    JUCE_LEAK_DETECTOR (PositionedImpulseResponse)
};
