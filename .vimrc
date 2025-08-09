set makeprg=make
set errorformat=%f:%l:%c:\ %m

let g:gdb_executable = 'bidi'
let g:gdb_arguments = '-r tests/graphics.lua'

nnoremap <leader>m :call LocalMake()<CR>
nnoremap <leader>r :execute '!./' . g:gdb_executable . ' ' . g:gdb_arguments<CR>
nnoremap <leader>bm :execute '!make && gdb -ex "break main" -ex "run" --args ' . g:gdb_executable . ' ' . g:gdb_arguments<CR>
nnoremap <leader>bl :execute '!make && gdb -ex "break ' . line('.') . '" -ex "run" --args ' . g:gdb_executable . ' ' . g:gdb_arguments<CR>

function! LocalMake()
	silent make

	" Filter non valid errors out of quicklist.
	let qfl = getqflist()
	let filtered = filter(copy(qfl), {_, entry -> entry.valid == 1})
	call setqflist(filtered, 'r')

	redraw!

	if len(filtered) > 0
		copen
	else
		cclose
	endif
endfunction
