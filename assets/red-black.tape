Output assets/red-black.gif

Set FontSize 16
Set Width 980
Set Height 560
Set Shell "bash"
Set Theme "Catppuccin Mocha"
Set FontFamily "Menlo"
Set Padding 24
Set WindowBar Colorful
Set BorderRadius 10
Set Margin 20
Set MarginFill "#181825"
Set TypingSpeed 40ms

Hide
Type `d=$(mktemp -d) && make -s example && c++ -std=c++11 -Isrc -Itests assets/demo/rb_steps.cpp -o $d/rb_steps && cp example $d/ && cd $d && export PS1="\[\e[38;5;117m\]$ \[\e[0m\]" && clear`
Enter
Show

Type "./rb_steps"
Sleep 400ms
Enter
Sleep 11s
