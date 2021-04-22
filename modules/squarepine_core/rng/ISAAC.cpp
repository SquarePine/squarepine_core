ISAAC::ISAAC (bool preseedResults) :
    count (maxNumItems)
{
    zerostruct (randMemory);
    zerostruct (results);

    if (preseedResults)
    {
        std::random_device device;
        std::mt19937 engine (device());
        std::uniform_int_distribution<uint32> dist (0, (uint32) std::numeric_limits<uint32>::max() - (uint32) 1);

        for (auto& r : results)
            r = dist (engine);
    }

    uint32 a, b, c, d, e, f, g, h;
    a = b = c = d = e = f = g = h = static_cast<uint32> (goldenRatio);

    //Perform the initial scramble:
    for (size_t i = 0; i < 4; ++i)
        shuffle (a, b, c, d, e, f, g, h);

    //Fill in randMemory[] with messy stuff:
    const size_t numShuffles = preseedResults ? 2 : 1;

    for (size_t m = 0; m < numShuffles; ++m)
    {
        for (size_t n = 0; n < maxNumItems; n += alpha)
        {
            if (preseedResults && ((m % 2) == 0))
            {
                a += results[n];
                b += results[n + 1];
                c += results[n + 2];
                d += results[n + 3];
                e += results[n + 4];
                f += results[n + 5];
                g += results[n + 6];
                h += results[n + 7];
            }

            shuffle (a, b, c, d, e, f, g, h);

            randMemory[n] = a;
            randMemory[n + 1] = b;
            randMemory[n + 2] = c;
            randMemory[n + 3] = d;
            randMemory[n + 4] = e;
            randMemory[n + 5] = f;
            randMemory[n + 6] = g;
            randMemory[n + 7] = h;
        }
    }

    next(); //Fill in the first set of results
}

uint32 ISAAC::generate()
{
    if (--count == 0)
    {
        next();
        count = maxNumItems - 1;
    }

    return results[count];
}

void ISAAC::shuffle (uint32& a, uint32& b, uint32& c, uint32& d,
                     uint32& e, uint32& f, uint32& g, uint32& h) noexcept
{
    shuffle<leftShift, 11u> (a, b, c, d);
    shuffle<rightShift, 2u> (b, c, d, e);
    shuffle<leftShift, 8u> (c, d, e, f);
    shuffle<rightShift, 16u> (d, e, f, g);
    shuffle<leftShift, 10u >(e, f, g, h);
    shuffle<rightShift, 4u> (f, g, h, a);
    shuffle<leftShift, 8u> (g, h, a, b);
    shuffle<rightShift, 9u> (h, a, b, c);
}

void ISAAC::next()
{
    ++randomValues.z;
    randomValues.y += randomValues.z;

    for (size_t i = 0; i < maxNumItems; ++i)
    {
        uint32 aa = randomValues.x;

        switch (i % 4)
        {
            case 0: aa ^= aa << 13; break;
            case 1: aa ^= aa >> 6; break;
            case 2: aa ^= aa << 2; break;
            case 3: aa ^= aa >> 16; break;

            default: jassertfalse; break;
        }

        const auto x = randMemory[i];
        randomValues.x = randMemory[(i + halfNumItems) % maxNumItems] + aa;

        const auto y = randMemory[i] = randMemory[(x >> 2) % maxNumItems] + aa + randomValues.y;
        randomValues.y = results[i] = randMemory[(y >> 10) % maxNumItems] + x;
    }
}
