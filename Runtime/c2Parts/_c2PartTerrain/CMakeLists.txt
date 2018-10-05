aux_source_directory(. DIR_LIBSRC_MATH)
add_library(math ${DIR_LIBSRC_MATH})

install (TARGETS math DESTINATION bin)
install (FILES math.h DESTINATION include)

