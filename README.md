# msh: a minimal Unix shell

**Name:** EDWIN WAWERU  
**Student ID:** SCT212-0110/2022  
**Course:** ICS 2305 Systems Programming

## Build and run

```bash
make
./msh
```

`make clean` removes the binary. The build uses `gcc -Wall -Wextra -g -std=c11` and compiles with no warnings.

## Features

| Task | Feature | Status |
|---|---|---|
| 1 | Prompt loop with `getline`; clean exit on Ctrl-D; no memory leaks (checked with Valgrind) | Complete |
| 2 | Splits input into arguments with `strtok`; ignores blank lines and extra whitespace | Complete |
| 3 | Runs external commands with `fork` / `execvp` / `waitpid`; reports failed calls | Complete |
| 4 | Built-ins: `cd` (no argument goes to `$HOME`), `help`, `exit` | Complete |
| 5 | Exit status of the previous command shown in the prompt, e.g. `msh[127]>`; 128 + signal number if killed by a signal | Complete |
| 6 | Background jobs with a trailing `&`; finished children reaped with `waitpid(-1, ..., WNOHANG)` | Complete |
| 7 | Output redirection with `>` using `open` and `dup2` (file created or truncated, mode 0644) | Complete |
| 8 | Ctrl-C stops the foreground command but not the shell | Complete |

Test cases and their recorded results are in [`tests/test_plan.md`](tests/test_plan.md).

## Design explanation

The shell prompts the user for input, parses the input, executes the command and stores the return status. At the start of each loop, any child processes which have completed are checked.

The input is read in using getline. Since the same buffer is reused on every loop and freed once the shell has finished, there are no memory leaks according to Valgrind. The line is split with strtok, splitting on spaces, tabs and line breaks. No copying is performed; strtok just places a ‘\0’ after every word. The pointers to these words are stored in an array, then a NULL is appended to the end so that the arguments can be passed to execvp as a NULL-terminated array.

The built-in commands are cd, help and exit. For cd, we need to change something within the shell itself. If it were executed in a child process, only the child process’s working directory would be changed. For exit, we want to terminate the shell; however, doing this in a child process would not affect the shell.

All remaining commands are executed in child processes. Once forked, the child calls execvp. If this function returns, the command couldn’t be executed, so an error message is printed and the child exits with exit code 127. If the command includes output redirection with >, the file is opened and the output is redirected with dup2. This must be done in the child process so that the shell’s own output is still shown on the terminal.

If the command is running in the foreground, the parent will wait for it to complete. The return status can be decoded with either WIFEXITED and WEXITSTATUS, or WIFSIGNALED and WTERMSIG. In the latter case, 128 is added to the signal number and stored in the shell. If the command is in the background (with an &), the PID is printed, but the parent does not wait. However, to prevent zombie processes, a loop calling waitpid(-1, &status, WNOHANG) is run before every prompt is printed, allowing any terminated children to be cleaned up.

SIGINT is ignored in the shell, so pressing Ctrl-C won’t cause it to exit. Within foreground child processes, the SIGINT action is reset to the default before calling execvp. Therefore, pressing Ctrl-C interrupts the running command, but the shell remains. However, within background child processes, SIGINT is still ignored, meaning the command will not be interrupted.

## Known limitations

- No pipes (`|`), input redirection (`<`) or append (`>>`).
- No quoting or escaping: `echo "two words"` is split into two arguments, including the quote characters.
- No `~` or `$VARIABLE` expansion: `cd ~/dir` fails (see test 15).
- At most 63 arguments per command; extra words are ignored.
- Background jobs are only reported when the next prompt is printed.
- No job control (`jobs`, `fg`, `bg`).
- Pressing Ctrl-C at an empty prompt has no visible effect.

## Sources and AI-tool declaration

- Course notes and lab sheets for ICS 2305 (process management, signals).
- Linux manual pages: `getline(3)`, `strtok(3)`, `fork(2)`, `execvp(3)`, `waitpid(2)`, `dup2(2)`, `open(2)`, `signal(2)`, `chdir(2)`.
- **Claude (Anthropic)** was used as a tutor throughout. It explained the concepts and reviewed my code for the Tasks. I integrated and built each step myself, ran every test and recorded the results, and wrote the design explanation in my own words.
