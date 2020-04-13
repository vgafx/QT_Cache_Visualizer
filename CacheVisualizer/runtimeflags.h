/*** RuntimeFlags
 * Simple class designed to keep track of certain settings / actions for the visualizer.
 * It is used to restrict certain actions based on the state of the simulation. For instance,
 * the user should be prevented from starting a simulation unless a memory trace is first
 * loaded (m_trace_loaded). The class is designed as a singleton, as the flags used should
 * be unique.
 ***/

#pragma once

enum SimulationMode{
    AUTOPLAY = 0, STEP_WISE
};

class RuntimeFlags
{
private:
    bool m_simulation_done;
    bool m_trace_loaded;
    bool m_config_loaded;
    bool m_sim_mode_selected;
    bool m_start_flag;
    bool m_pause_flag;
    bool m_worker_running;
    SimulationMode m_sim_mode;

    RuntimeFlags();

    /*Setters & Getters that modify the actual object*/
    bool getSimulationDoneImpl() const;
    void setSimulationDoneImpl(bool done);

    bool getTraceLoadedImpl() const;
    void setTraceLoadedImpl(bool tloaded);

    bool getConfigLoadedImpl() const;
    void setConfigLoadedImpl(bool cloaded);

    bool getSimModeSelectedImpl() const;
    void setSimModeSelectedImpl(bool selected);

    bool getStartFlagImpl() const;
    void setStartFlagImpl(bool sflag);

    bool getPauseFlagImpl() const;
    void setPauseFlagImpl(bool pflag);

    bool getWorkerRunningImpl() const;
    void setWorkerRunningImpl(bool running);

    SimulationMode getSimModeImpl() const;
    void setSimModeImpl(SimulationMode mode);

    void resetFlagsImpl();

public:
    /*No copy constructor*/
    RuntimeFlags(const RuntimeFlags&) = delete;
    /*No assignment operator*/
    RuntimeFlags& operator= (const RuntimeFlags&) = delete;

    static RuntimeFlags& GetInstance();

    /*Statically accessible Setters & Getters that call the equivalent implementation*/
    static bool getSimulationDone();
    static void setSimulationDone(bool done);

    static bool getTraceLoaded();
    static void setTraceLoaded(bool tloaded);

    static bool getConfigLoaded();
    static void setConfigLoaded(bool cloaded);

    static bool getSimModeSelected();
    static void setSimModeSelected(bool selected);

    static bool getStartFlag();
    static void setStartFlag(bool sflag);

    static bool getPauseFlag();
    static void setPauseFlag(bool pflag);

    static bool getWorkerRunning();
    static void setWorkerRunning(bool running);

    static SimulationMode getSimMode();
    static void setSimMode(SimulationMode mode);

    static void resetFlags();

};
