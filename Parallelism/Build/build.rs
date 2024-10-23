/*
    * build.rs
    *
    * this file is used to compile the scheduler.c file and create a static library
*/

fn main() {
    cc::Build::new()
        .file("src/scheduler.c")
        .compile("scheduler");
}

extern "C" {
    fn scheduler();
}

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

impl
