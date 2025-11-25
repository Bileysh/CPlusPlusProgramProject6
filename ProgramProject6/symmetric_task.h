#pragma once
#include <coroutine>
#include <exception>

using namespace std;

class SymmetricTask {
public:
    struct promise_type;
    using handle = coroutine_handle<promise_type>;

    SymmetricTask(handle h) : coro(h) {}
    ~SymmetricTask() {
        if (coro) coro.destroy(); 
    }

    SymmetricTask(const SymmetricTask&) = delete;
    SymmetricTask& operator=(const SymmetricTask&) = delete;

    SymmetricTask(SymmetricTask&& other) : coro(other.coro) { 
        other.coro = nullptr; 
    }

    void resume() { 
        if (coro) coro.resume(); 
    }
    bool is_done() const {
        return !coro || coro.done(); 
    }

    struct TransferAwaiter {
        handle target_coro; 

        bool await_ready() const noexcept { 
            return false; 
        } 


        coroutine_handle<> await_suspend(coroutine_handle<> caller) noexcept {
            target_coro.promise().continuation = caller;
            return target_coro;
        }

        void await_resume() noexcept {}
    };

    auto operator co_await() {
        return TransferAwaiter{ 
            coro 
        };
    }

    struct promise_type {
        coroutine_handle<> continuation = nullptr; 

        auto get_return_object() {
            return SymmetricTask{ 
                handle::from_promise(*this) 
            }; 
        }
        auto initial_suspend() { 
            return suspend_always{}; 
        } 

        struct FinalAwaiter {
            bool await_ready() const noexcept { 
                return false;
            }
            coroutine_handle<> await_suspend(handle h) noexcept {
                auto prev = h.promise().continuation;
                if (prev) return prev;
                return noop_coroutine();
            }
            void await_resume() noexcept {}
        };

        auto final_suspend() noexcept { 
            return FinalAwaiter{};
        }
        void return_void() {}
        void unhandled_exception() {
            terminate(); 
        }
    };

private:
    handle coro;
};

void run_processing_demo();