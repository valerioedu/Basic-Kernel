use std::sync::{Arc, Mutex, RwLock};
use std::thread;
use std::time::{Duration, Instant};
use std::collections::HashMap;
use std::ffi::c_void;
use std::os::raw::{c_int, c_ulonglong};

// Previous FFI declarations remain the same...
#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub enum ProcessState {
    Ready,
    Waiting,
    Running,
    Terminated,
}

#[repr(C)]
#[derive(Copy, Clone, Debug, PartialEq)]
pub enum ProcessPriority {
    Zero,
    Normal,
    Max,
}

// Previous struct definitions remain the same...
#[repr(C)]
pub struct Process {
    pid: u64,
    size: u64,
    parent: *mut Process,
    state: ProcessState,
    priority: ProcessPriority,
    pages: *mut Page,
    terminated: bool,
    ctx: Context,
    next: *mut Process,
}

// New struct to track core metrics
struct CoreMetrics {
    load: f32,                    // Current load (0.0 - 1.0)
    process_count: usize,         // Number of processes
    high_priority_count: usize,   // Number of high priority processes
    last_migration: Instant,      // Time of last process migration
}

// New struct to track process metrics
struct ProcessMetrics {
    cpu_usage: f32,              // CPU usage (0.0 - 1.0)
    memory_usage: u64,           // Memory usage in bytes
    priority: ProcessPriority,    // Process priority
    core_id: usize,              // Current core
    last_scheduled: Instant,     // Last time the process was scheduled
}

pub struct AutomaticScheduler {
    core_count: usize,
    process_lists: Vec<Arc<RwLock<Vec<*mut Process>>>>,
    core_metrics: Arc<RwLock<Vec<CoreMetrics>>>,
    process_metrics: Arc<RwLock<HashMap<u64, ProcessMetrics>>>,
    balancing_interval: Duration,
}

impl AutomaticScheduler {
    pub fn new(core_count: usize) -> Self {
        let mut process_lists = Vec::with_capacity(core_count);
        let mut core_metrics = Vec::with_capacity(core_count);

        for _ in 0..core_count {
            process_lists.push(Arc::new(RwLock::new(Vec::new())));
            core_metrics.push(CoreMetrics {
                load: 0.0,
                process_count: 0,
                high_priority_count: 0,
                last_migration: Instant::now(),
            });
        }

        AutomaticScheduler {
            core_count,
            process_lists,
            core_metrics: Arc::new(RwLock::new(core_metrics)),
            process_metrics: Arc::new(RwLock::new(HashMap::new())),
            balancing_interval: Duration::from_millis(100),
        }
    }

    fn start_load_balancer(&self) {
        let core_metrics = Arc::clone(&self.core_metrics);
        let process_metrics = Arc::clone(&self.process_metrics);
        let process_lists = self.process_lists.clone();
        let balancing_interval = self.balancing_interval;

        thread::spawn(move || {
            loop {
                thread::sleep(balancing_interval);
                Self::balance_loads(
                    &core_metrics,
                    &process_metrics,
                    &process_lists,
                );
            }
        });
    }

    fn balance_loads(
        core_metrics: &RwLock<Vec<CoreMetrics>>,
        process_metrics: &RwLock<HashMap<u64, ProcessMetrics>>,
        process_lists: &[Arc<RwLock<Vec<*mut Process>>>],
    ) {
        let mut metrics = core_metrics.write().unwrap();
        let mut processes = process_metrics.write().unwrap();

        // Find overloaded and underloaded cores
        let mut overloaded_cores = Vec::new();
        let mut underloaded_cores = Vec::new();

        for (core_id, metric) in metrics.iter().enumerate() {
            if metric.load > 0.8 {
                overloaded_cores.push(core_id);
            } else if metric.load < 0.3 {
                underloaded_cores.push(core_id);
            }
        }

        // Balance loads by migrating processes
        for &from_core in &overloaded_cores {
            if let Some(&to_core) = underloaded_cores.first() {
                // Find suitable process to migrate
                let processes_to_migrate = Self::select_processes_for_migration(
                    from_core,
                    &processes,
                    &metrics[from_core],
                );

                for pid in processes_to_migrate {
                    Self::migrate_process(
                        pid,
                        from_core,
                        to_core,
                        &mut processes,
                        process_lists,
                    );

                    // Update metrics
                    metrics[from_core].process_count -= 1;
                    metrics[to_core].process_count += 1;
                    
                    if processes[&pid].priority == ProcessPriority::Max {
                        metrics[from_core].high_priority_count -= 1;
                        metrics[to_core].high_priority_count += 1;
                    }
                }
            }
        }
    }

    fn select_processes_for_migration(
        core_id: usize,
        processes: &HashMap<u64, ProcessMetrics>,
        core_metric: &CoreMetrics,
    ) -> Vec<u64> {
        let mut candidates = processes
            .iter()
            .filter(|(_, p)| {
                p.core_id == core_id && 
                p.priority != ProcessPriority::Max &&  // Don't migrate high priority processes
                p.last_scheduled.elapsed() > Duration::from_millis(500)  // Don't migrate recently scheduled processes
            })
            .map(|(&pid, _)| pid)
            .collect::<Vec<_>>();

        // Sort by CPU usage (migrate less CPU-intensive processes first)
        candidates.sort_by(|&a, &b| {
            processes[&a].cpu_usage.partial_cmp(&processes[&b].cpu_usage).unwrap()
        });

        candidates.truncate(core_metric.process_count / 4);  // Migrate up to 25% of processes
        candidates
    }

    fn migrate_process(
        pid: u64,
        from_core: usize,
        to_core: usize,
        processes: &mut HashMap<u64, ProcessMetrics>,
        process_lists: &[Arc<RwLock<Vec<*mut Process>>>],
    ) {
        // Update process metrics
        if let Some(process) = processes.get_mut(&pid) {
            process.core_id = to_core;
            process.last_scheduled = Instant::now();
        }

        // Move process between lists
        let mut from_list = process_lists[from_core].write().unwrap();
        let mut to_list = process_lists[to_core].write().unwrap();

        if let Some(pos) = from_list.iter().position(|p| unsafe { (*(*p)).pid == pid }) {
            let process = from_list.remove(pos);
            to_list.push(process);
        }
    }

    pub fn start(&self) {
        // Start the load balancer thread
        self.start_load_balancer();

        // Start scheduler threads for each core
        let mut handles = Vec::new();

        for core_id in 0..self.core_count {
            let process_list = Arc::clone(&self.process_lists[core_id]);
            let core_metrics = Arc::clone(&self.core_metrics);
            let process_metrics = Arc::clone(&self.process_metrics);

            let handle = thread::spawn(move || {
                // Pin thread to core
                #[cfg(target_os = "linux")]
                {
                    use core_affinity::set_for_current;
                    let core_ids = core_affinity::get_core_ids().unwrap();
                    set_for_current(core_ids[core_id]);
                }

                loop {
                    // Update metrics before scheduling
                    Self::update_metrics(
                        core_id,
                        &process_list,
                        &core_metrics,
                        &process_metrics,
                    );

                    // Convert process list to C-compatible format and run scheduler
                    let mut c_process_list = Self::prepare_process_list(&process_list);
                    unsafe {
                        scheduler(c_process_list.as_mut_ptr());
                    }

                    thread::sleep(Duration::from_micros(100));
                }
            });

            handles.push(handle);
        }

        // Wait for all threads
        for handle in handles {
            let _ = handle.join();
        }
    }

    fn update_metrics(
        core_id: usize,
        process_list: &RwLock<Vec<*mut Process>>,
        core_metrics: &RwLock<Vec<CoreMetrics>>,
        process_metrics: &RwLock<HashMap<u64, ProcessMetrics>>,
    ) {
        let processes = process_list.read().unwrap();
        let mut metrics = core_metrics.write().unwrap();
        let mut proc_metrics = process_metrics.write().unwrap();

        let mut high_priority = 0;
        let mut total_cpu = 0.0;

        for &process in processes.iter() {
            unsafe {
                let pid = (*process).pid;
                let priority = (*process).priority;
                
                // Update process metrics
                let cpu_usage = Self::calculate_cpu_usage(process);
                proc_metrics.entry(pid).and_modify(|e| {
                    e.cpu_usage = cpu_usage;
                    e.last_scheduled = Instant::now();
                }).or_insert(ProcessMetrics {
                    cpu_usage,
                    memory_usage: (*process).size,
                    priority,
                    core_id,
                    last_scheduled: Instant::now(),
                });

                total_cpu += cpu_usage;
                if priority == ProcessPriority::Max {
                    high_priority += 1;
                }
            }
        }

        // Update core metrics
        metrics[core_id] = CoreMetrics {
            load: total_cpu / self.core_count as f32,
            process_count: processes.len(),
            high_priority_count: high_priority,
            last_migration: metrics[core_id].last_migration,
        };
    }

    fn calculate_cpu_usage(process: *mut Process) -> f32 {
        // This is a simplified CPU usage calculation
        // In a real implementation, you'd want to track actual CPU time
        unsafe {
            match (*process).state {
                ProcessState::Running => 1.0,
                ProcessState::Ready => 0.5,
                ProcessState::Waiting => 0.2,
                ProcessState::Terminated => 0.0,
            }
        }
    }

    fn prepare_process_list(process_list: &RwLock<Vec<*mut Process>>) -> Vec<*mut Process> {
        let processes = process_list.read().unwrap();
        let mut c_list = vec![std::ptr::null_mut(); 128];
        
        for (i, &process) in processes.iter().enumerate() {
            if i >= 128 { break; }
            c_list[i] = process;
        }
        
        c_list
    }

    pub fn create_process(&self, size: u64, priority: ProcessPriority, entry_point: extern "C" fn()) -> Result<u64, &'static str> {
        // Find the least loaded core
        let metrics = self.core_metrics.read().unwrap();
        let target_core = metrics.iter()
            .enumerate()
            .min_by(|(_, a), (_, b)| a.load.partial_cmp(&b.load).unwrap())
            .map(|(i, _)| i)
            .unwrap_or(0);

        // Create the process
        unsafe {
            let process = create(
                self.generate_pid(),
                size,
                priority,
                entry_point
            );

            if process.is_null() {
                return Err("Failed to create process");
            }

            // Add to process list
            let mut process_list = self.process_lists[target_core].write().unwrap();
            process_list.push(process);

            Ok((*process).pid)
        }
    }

    fn generate_pid(&self) -> u64 {
        // Simple PID generation - in practice, you'd want something more sophisticated
        let metrics = self.process_metrics.read().unwrap();
        (metrics.len() as u64) + 1
    }
}

// Example usage
#[no_mangle]
pub extern "C" fn example_process() {
    println!("Process running on core {}", core_affinity::get_core_id().unwrap().id);
    // Simulate some work
    thread::sleep(Duration::from_millis(100));
}

fn main() {
    let core_count = core_affinity::get_core_ids().unwrap().len();
    let scheduler = AutomaticScheduler::new(core_count);

    // Create some test processes
    for _ in 0..20 {
        scheduler.create_process(
            4096,
            ProcessPriority::Normal,
            example_process
        ).unwrap();
    }

    // Create some high-priority processes
    for _ in 0..5 {
        scheduler.create_process(
            4096,
            ProcessPriority::Max,
            example_process
        ).unwrap();
    }

    // Start the automatic scheduler
    scheduler.start();
}

/*
*   multithreading.rs
*
*   this file is AI-generated since this is my first time working with Rust
*
*   I will try to understand the code and make it better
*/