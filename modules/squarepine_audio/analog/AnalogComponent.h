//==============================================================================
/** */
using AnalogMatrix = juce::dsp::Matrix<double>;

//==============================================================================
/** */
struct AnalogComponent final
{
    //==============================================================================
    /** */
    enum class Type
    {
        input,
        output,
        potentiometer,
        opa,
        resistor,
        capacitor,
        inductor,
        diode,
        transistor,
        triode
    };

    /** Non-linear function type.

        This will receive the voltages vector 'v' and must write
        the currents vector 'i' and the Jacobian matrix 'J'.
    */
    using NonlinearFunc = std::function<void (const AnalogMatrix& v, AnalogMatrix& i, AnalogMatrix& J)>;

    //==============================================================================
    /** */
    AnalogComponent() = default;

    /** */
    AnalogComponent (Type type_,
                     const String& name_,
                     double value_ = 0.0,
                     Array<int> nodes_ = {},
                     int numPorts_ = 0,
                     NonlinearFunc model_ = nullptr) :
        type (type_),
        name (name_),
        value (value_),
        nodes (nodes_),
        numPorts (numPorts_),
        model (model_)
    {
    }

    /** */
    AnalogComponent (const AnalogComponent& other) :
        type (other.type),
        name (other.name),
        value (other.value),
        nodes (other.nodes),
        numPorts (other.numPorts),
        model (other.model)
    {
    }

    /** */
    AnalogComponent (AnalogComponent&& other) :
        type (other.type),
        name (other.name),
        value (other.value),
        nodes (other.nodes),
        numPorts (other.numPorts),
        model (other.model)
    {
    }

    /** */
    AnalogComponent& operator= (const AnalogComponent& other)
    {
        type = other.type;
        name = other.name;
        value = other.value;
        nodes = other.nodes;
        numPorts = other.numPorts;
        model = other.model;
        return *this;
    }

    /** */
    AnalogComponent& operator= (AnalogComponent&& other)
    {
        type = other.type;
        name = other.name;
        value = other.value;
        nodes = other.nodes;
        numPorts = other.numPorts;
        model = other.model;
        return *this;
    }

    //==============================================================================
    Type type = Type::input;
    String name;
    double value = 0.0;
    Array<int> nodes;
    int numPorts = 0;
    NonlinearFunc model = nullptr;
};

//==============================================================================
namespace AnalogComponentFactory
{
    /**

        @param name
        @param nodePositive
        @param nodeNegative

        @returns
    */
    AnalogComponent makeInput (const String& name, int nodePositive, int nodeNegative)
    {
        return { AnalogComponent::Type::input, name, 0.0, { nodePositive, nodeNegative }, 1 };
    }

    /**

        @param name
        @param nodePositive
        @param nodeNegative

        @returns
    */
    AnalogComponent makeOutput (const String& name, int nodePositive, int nodeNegative)
    {
        return { AnalogComponent::Type::output, name, 0.0, { nodePositive, nodeNegative }, 1 };
    }

    /**

        @param maxResistance
        @param name
        @param nodePositive
        @param nodeNegative
        @param nodeTap

        @returns
    */
    AnalogComponent makePotentiometer (double maxResistance, const String& name, int nodePositive, int nodeNegative, int nodeTap)
    {
        return { AnalogComponent::Type::potentiometer, name, maxResistance, { nodePositive, nodeNegative, nodeTap }, 2 };
    }

    /**

        @param gain
        @param name
        @param nodeInPositive
        @param nodeInNegative
        @param nodeOut

        @returns
    */
    AnalogComponent makeOPA (double gain, const String& name, int nodeInPositive, int nodeInNegative, int nodeOut)
    {
        return { AnalogComponent::Type::input, name, gain, { nodeInPositive, nodeInNegative, nodeOut }, 1 };
    }

    /**

        @param resistance
        @param name
        @param nodePositive
        @param nodeNegative

        @returns
    */
    AnalogComponent makeResistor (double resistance, const String& name, int nodePositive, int nodeNegative)
    {
        return { AnalogComponent::Type::resistor, name, resistance, { nodePositive, nodeNegative }, 1 };
    }

    /**

        @param capacitance
        @param name
        @param nodePositive
        @param nodeNegative

        @returns
    */
    AnalogComponent makeCapacitor (double capacitance, const String& name,  int nodePositive, int nodeNegative)
    {
        return { AnalogComponent::Type::capacitor, name, capacitance, { nodePositive, nodeNegative }, 1 };
    }

    /**

        @param inductance
        @param name
        @param nodePositive
        @param nodeNegative

        @returns
    */
    AnalogComponent makeInductor (double inductance, const String& name, int nodePositive, int nodeNegative)
    {
        return { AnalogComponent::Type::inductor, name, inductance, { nodePositive, nodeNegative }, 1 };
    }

    /**

        @param name
        @param nodeBase
        @param nodeColector
        @param nodeEmiter

        @returns
    */
    AnalogComponent makeTransistor (const String& name, int nodeBase, int nodeColector, int nodeEmiter)
    {
        return
        {
            AnalogComponent::Type::transistor, name, 0.0,
            { nodeBase, nodeEmiter, nodeColector, nodeEmiter }, 2,
            [] (const AnalogMatrix& v, AnalogMatrix& i, AnalogMatrix& J)
            {
                i = v;
                J = AnalogMatrix::identity (std::max (v.getNumRows(), v.getNumColumns()));
            }
        };
    }

    /**

        @param name
        @param nodeGrid
        @param nodePlate
        @param nodeCathode

        @returns
    */
    AnalogComponent makeTriode (const String& name, int nodeGrid, int nodePlate, int nodeCathode)
    {
        return
        {
            AnalogComponent::Type::triode, name, 0.0, { nodeGrid, nodeCathode, nodePlate, nodeCathode }, 2,
            [] (const AnalogMatrix& v, AnalogMatrix& i, AnalogMatrix& J)
            {
                constexpr auto Gg       = 0.000606;
                constexpr auto xi       = 1.354;
                constexpr auto Cg       = 13.9;
                constexpr auto Gp       = 0.00214;
                constexpr auto gamma    = 1.303;
                constexpr auto Cp       = 3.04;
                constexpr auto mu       = 100.8;
                constexpr auto dIg_dVpk = 0.0;

                auto Vgk      = v (0, 0);
                auto Vpk      = v (1, 0);
                auto expg     = std::exp (Cg * Vgk);
                auto logg     = std::log (1.0 + expg) / Cg;
                auto Ig       = -Gg * std::pow (logg, xi);
                auto dIg_dVgk = -Gg * xi * std::pow (logg, xi - 1.0) * expg / (1.0 + expg);
                auto expp     = std::exp (Cp * ((Vpk / mu) + Vgk));
                auto logp     = std::log (1.0 + expp) / Cp;
                auto Ip       = -Gp * std::pow (logp, gamma) - Ig;
                auto dIp_dVgk = -Gp * gamma * std::pow (logp, gamma - 1.0) * expp / (1.0 + expp);
                auto dIp_dVpk = dIp_dVgk / mu;

                i (0, 0) = Ig;
                i (1, 0) = Ip;
                J (0, 0) = dIg_dVgk;
                J (0, 1) = dIg_dVpk;
                J (1, 0) = dIp_dVgk;
                J (1, 1) = dIp_dVpk;
            }
        };
    }
}
