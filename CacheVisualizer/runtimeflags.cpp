#include "runtimeflags.h"

/*Default ctor*/
RuntimeFlags::RuntimeFlags()
{
    m_simulation_done = false;
    m_trace_loaded = false;
    m_config_loaded = false;
    m_sim_mode_selected = false;
    m_start_flag = false;
    m_pause_flag = false;
    m_worker_running = false;
    m_sim_mode = STEP_WISE;
}

/*Internal implementations of Getters & Setters*/
bool RuntimeFlags::getSimulationDoneImpl() const
{
    return m_simulation_done;
}

void RuntimeFlags::setSimulationDoneImpl(bool done)
{
    m_simulation_done = done;
}

bool RuntimeFlags::getTraceLoadedImpl() const
{
    return m_trace_loaded;
}

void RuntimeFlags::setTraceLoadedImpl(bool tloaded)
{
    m_trace_loaded = tloaded;
}

bool RuntimeFlags::getConfigLoadedImpl() const
{
    return m_config_loaded;
}

void RuntimeFlags::setConfigLoadedImpl(bool cloaded)
{
    m_config_loaded = cloaded;
}

bool RuntimeFlags::getSimModeSelectedImpl() const
{
    return  m_sim_mode_selected;
}

void RuntimeFlags::setSimModeSelectedImpl(bool selected)
{
    m_sim_mode_selected = selected;
}

bool RuntimeFlags::getStartFlagImpl() const
{
    return m_start_flag;
}

void RuntimeFlags::setStartFlagImpl(bool sflag)
{
    m_start_flag = sflag;
}

bool RuntimeFlags::getPauseFlagImpl() const
{
    return m_pause_flag;
}

void RuntimeFlags::setPauseFlagImpl(bool pflag)
{
    m_pause_flag = pflag;
}

bool RuntimeFlags::getWorkerRunningImpl() const
{
    return m_worker_running;
}

void RuntimeFlags::setWorkerRunningImpl(bool running)
{
    m_worker_running = running;
}

SimulationMode RuntimeFlags::getSimModeImpl() const
{
    return m_sim_mode;
}

void RuntimeFlags::setSimModeImpl(SimulationMode mode)
{
    m_sim_mode = mode;
}

void RuntimeFlags::resetFlagsImpl()
{
    m_simulation_done = false;
    m_trace_loaded = false;
    m_config_loaded = false;
    m_sim_mode_selected = false;
    m_start_flag = false;
    m_pause_flag = false;
    m_worker_running = false;
}

/*Singleton object creaion*/
RuntimeFlags &RuntimeFlags::GetInstance()
{
    static RuntimeFlags instance;
    return instance;
}

/*Publicaly accessible Setters & Getters*/
bool RuntimeFlags::getSimulationDone()
{
    return GetInstance().getSimulationDoneImpl();
}

void RuntimeFlags::setSimulationDone(bool done)
{
    GetInstance().setSimulationDoneImpl(done);
}

bool RuntimeFlags::getTraceLoaded()
{
    return GetInstance().getTraceLoadedImpl();
}

void RuntimeFlags::setTraceLoaded(bool tloaded)
{
    GetInstance().setTraceLoadedImpl(tloaded);
}

bool RuntimeFlags::getConfigLoaded()
{
    return  GetInstance().getConfigLoadedImpl();
}

void RuntimeFlags::setConfigLoaded(bool cloaded)
{
    GetInstance().setConfigLoadedImpl(cloaded);
}

bool RuntimeFlags::getSimModeSelected()
{
    return GetInstance().getSimModeSelectedImpl();
}

void RuntimeFlags::setSimModeSelected(bool selected)
{
    GetInstance().setSimModeSelectedImpl(selected);
}

bool RuntimeFlags::getStartFlag()
{
    return GetInstance().getStartFlagImpl();
}

void RuntimeFlags::setStartFlag(bool sflag)
{
    GetInstance().setStartFlagImpl(sflag);
}

bool RuntimeFlags::getPauseFlag()
{
    return GetInstance().getPauseFlagImpl();
}

void RuntimeFlags::setPauseFlag(bool pflag)
{
    GetInstance().setPauseFlagImpl(pflag);
}

bool RuntimeFlags::getWorkerRunning()
{
    return GetInstance().getWorkerRunningImpl();
}

void RuntimeFlags::setWorkerRunning(bool running)
{
    GetInstance().setWorkerRunningImpl(running);
}

SimulationMode RuntimeFlags::getSimMode()
{
    return GetInstance().getSimModeImpl();
}

void RuntimeFlags::setSimMode(SimulationMode mode)
{
    GetInstance().setSimModeImpl(mode);
}

void RuntimeFlags::resetFlags()
{
    GetInstance().resetFlagsImpl();
}

