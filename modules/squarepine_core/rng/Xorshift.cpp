Xorshift::Xorshift() noexcept
{
    seed (static_cast<uint32> (Time::currentTimeMillis()));
}

Xorshift::Xorshift (uint32 s) noexcept
{
    seed (s);
}

//==============================================================================
uint32 Xorshift::generate() noexcept
{
    auto t = components.x;
    t ^= t << 11;
    t ^= t >> 8;
    components.x = components.y;
    components.y = components.z;
    components.z = components.w;
    components.w ^= components.w >> 19;
    components.w ^= t;
    return components.w;
}

void Xorshift::seed (uint32 s) noexcept
{
    components = Vector4D (s << 11, s >> 3, s << 5, s >> 2);
}
