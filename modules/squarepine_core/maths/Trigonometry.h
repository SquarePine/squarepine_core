//==============================================================================
/** Cas function */
inline double cas (double x) noexcept       { return std::sin (x) + std::cos (x); }

/** Cas function */
inline float cas (float x) noexcept         { return std::sin (x) + std::cos (x); }

/** Cas function */
template<typename Type>
inline Type cas (Type x) noexcept           { return (Type) cas ((double) x); }

//==============================================================================
/** Cotangent function */
inline double cot (double x) noexcept       { return std::cos (x) / std::sin (x); }

/** Cotangent function */
inline float cot (float x) noexcept         { return std::cos (x) / std::sin (x); }

/** Cotangent function */
template<typename Type>
inline Type cot (Type x) noexcept           { return (Type) cot ((double) x); }

//==============================================================================
/** Cosecant function */
inline double csc (double x) noexcept       { return 1.0 / std::sin (x); }

/** Cosecant function */
inline float csc (float x) noexcept         { return 1.0f / std::sin (x); }

/** Cosecant function */
template<typename Type>
inline Type csc (Type x) noexcept           { return (Type) csc ((double) x); }

//==============================================================================
/** Secant function */
inline double sec (double x) noexcept       { return 1.0 / std::cos (x); }

/** Secant function */
inline float sec (float x) noexcept         { return 1.0f / std::cos (x); }

/** Secant function */
template<typename Type>
inline Type sec (Type x) noexcept           { return (Type) sec ((double) x); }

//==============================================================================
/** Versine function */
inline double versin (double x) noexcept    { return 1.0 - std::cos (x); }

/** Versine function */
inline float versin (float x) noexcept      { return 1.0f - std::cos (x); }

/** Versine function */
template<typename Type>
inline Type versin (Type x) noexcept        { return (Type) versin ((double) x); }

//==============================================================================
/** Vercosine function */
inline double vercosin (double x) noexcept  { return 1.0 + std::cos (x); }

/** Vercosine function */
inline float vercosin (float x) noexcept    { return 1.0f + std::cos (x); }

/** Vercosine function */
template<typename Type>
inline Type vercosin (Type x) noexcept      { return (Type) vercosin ((double) x); }

//==============================================================================
/** Coversine function */
inline double coversin (double x) noexcept  { return 1.0 + std::cos (x); }

/** Coversine function */
inline float coversin (float x) noexcept    { return 1.0f + std::cos (x); }

/** Coversine function */
template<typename Type>
inline Type coversin (Type x) noexcept      { return (Type) vercosin ((double) x); }

//==============================================================================
/** Exsecant function */
inline double exsec (double x) noexcept     { return sec (x) - 1.0; }

/** Exsecant function */
inline float exsec (float x) noexcept       { return sec (x) - 1.0f; }

/** Exsecant function */
template<typename Type>
inline Type exsec (Type x) noexcept         { return (Type) exsec ((double) x); }

//==============================================================================
/** Excosecant function */
inline double excsc (double x) noexcept     { return csc (x) - 1.0; }

/** Excosecant function */
inline float excsc (float x) noexcept       { return csc (x) - 1.0f; }

/** Excosecant function */
template<typename Type>
inline Type excsc (Type x) noexcept         { return (Type) excsc ((double) x); }

