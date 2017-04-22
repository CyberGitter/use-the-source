<?php

//wp-includes/functions.php
function wp_parse_args( $args, $defaults = '' ) {
	if ( is_object( $args ) )
		$r = get_object_vars( $args ); //将对象键/值展开并存入数组
	elseif ( is_array( $args ) )
		$r =& $args;//直接指向该数组
	else
		wp_parse_str( $args, $r );//将$args分解，存入$r

	if ( is_array( $defaults ) )
		return array_merge( $defaults, $r );//将数组$r的键值依次附加在数组$defaults后面。如果二者中存在同名的键，则$r的键会覆盖$defaults的键
	return $r;
}//wp_parse_args()总的作用就是，将$args的键值对覆盖$defaults的同名键值对，并返回之

//wp-includes/formatting.php
function wp_parse_str( $string, &$array ) {
	parse_str( $string, $array );//将$string分解，存入$array
	if ( get_magic_quotes_gpc() )
		$array = stripslashes_deep( $array );
	$array = apply_filters( 'wp_parse_str', $array );
}