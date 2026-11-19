# msh Test Plan

| # | Input | Expected behaviour | Actual result |
|---|---|---|---|
| 1 | (blank line) | New prompt, no error | New prompt `msh[0]>` appeared; nothing printed; status unchanged. ✅ Pass |
| 2 | `echo hello world` | Prints text; status 0 | Printed `hello world` once; next prompt `msh[0]>`. ✅ Pass |
| 3 | `ls /nonexistent` | Error from ls; non-zero status | ls printed "cannot access '/nonexistent': No such file or directory"; next prompt `msh[2]>` (ls exit code 2). ✅ Pass |
| 4 | `nosuchcommand` | Error message; status 127 | Printed "nosuchcommand: No such file or directory" (from perror after execvp failed); next prompt `msh[127]>`. ✅ Pass |
| 5 | `cd /tmp` then `pwd` | Prints /tmp | After `cd /tmp` prompt showed `msh[0]>`; `pwd` printed `/tmp`. ✅ Pass |
| 6 | `sleep 3 &` | Prompt returns at once; PID shown | Printed `[4353]` and the prompt returned immediately. Later, before the next prompt, msh reported `[4353] done, status 0` (child reaped, no zombie). ✅ Pass |
| 7 | `echo hi > f.txt` then `cat f.txt` | File contains hi | `echo` printed nothing to the screen (output went to the file); `cat f.txt` printed `hi`. (Run from /tmp, so the file was /tmp/f.txt.) ✅ Pass |
| 8 | `sleep 100` then Ctrl-C | Sleep stops; shell survives; status 130 | `^C` stopped sleep immediately; the shell stayed alive; next prompt `msh[130]>` (128 + SIGINT 2). ✅ Pass |
| 9 | Ctrl-D at the prompt | Shell exits cleanly | Pressing Ctrl-D at `msh[0]>` printed a newline and returned to the bash prompt. ✅ Pass |
| 10 | `help` | Prints built-in usage guide; status 0 | Printed the guide listing cd, help, exit, `&` and `>`; next prompt `msh[0]>`. ✅ Pass |
| 11 | `echo first > out.txt` then `echo second > out.txt` then `cat out.txt` | Overwrites file; prints `second` only; status 0 | `cat out.txt` printed only `second` (O_TRUNC emptied the file before the second write); prompt `msh[0]>`. ✅ Pass |
| 12 | `> out.txt` | No command executed; no file created; status 2 | After deleting out.txt, `> out.txt` gave prompt `msh[2]>`; `ls out.txt` reported "No such file or directory", so no file was created. ✅ Pass |
| 13 | `&` | Returns fresh prompt immediately; no error | Prompt returned immediately with no error and no process started; status stayed at its previous value (`msh[2]>`). ✅ Pass |
| 14 | `exit` | Shell terminates cleanly; back at the bash prompt; `echo $?` shows 0 | `exit` returned to `wins@py`; `echo $?` printed `0`. ✅ Pass |
| 15 | `cd ~/winsdev/assignments/msh-SCT212-0110-2022` | (Exploratory) see how msh handles `~` | Printed "cd: No such file or directory"; prompt `msh[1]>`. msh does not expand `~` (bash normally does). Recorded as a known limitation. ⚠️ Limitation |