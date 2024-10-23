/*
    * multithreading.rs
    *
    * i'll write a scheduler in c, however I'll multithread it in rust to make it easier to write
    *
*/


#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ThreadStatus {
    Ready,
    Running,
    Blocked,
    Finished,
}

pub struct Thread {
    id: usize,
    stack: Vec<u8>,
    status: ThreadStatus,
    entry_point: fn(),
}

impl Thread {
    pub fn new(id: usize, entry_point, fn()) -> Self {
        let stack_size = 128 * 1024 * 1024;
        let stack = vec![0; stack_size];
        Thread {
            id,
            stack,
            status: ThreadStatus::Ready,
            entry_point,
        }
    }
}

pub struct Scheduler {
    threads: Vec<Thread>,
    current_thread: usize,
}

impl Scheduler {
    pub fn new() -> Self {
        Scheduler {
            threads: Vec::new(),
            current_thread: None,
        }
    }

    pub fn add_thread(&mut self, thread: Thread) {
        self.threads.push(thread);
    }
}

pub enum Threads {
    Thread_one,
    Thread_two,
    Thread_three,
    Thread_four
}