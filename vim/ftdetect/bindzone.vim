au BufNewFile,BufRead *
\ if expand('%:p') =~ '/etc/bind/' |
\   set filetype=bindzone |
\ endif
