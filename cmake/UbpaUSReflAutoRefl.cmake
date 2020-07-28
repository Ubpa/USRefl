function(Ubpa_USReflAutoRefl src dst)
  add_custom_command(
	OUTPUT ${dst}
	COMMAND Ubpa::USRefl_AutoRefl ARGS ${src} ${dst}
	DEPENDS Ubpa::USRefl_AutoRefl
  )
endfunction()
