<?php
	/*parse_request()接收被访问的URL，设置好query_vars供接下来的$wp_the_query->query(query_vars)调用。在$wp_the_query->query(query_vars)中的get_posts()中，条件标签被赋值，然后$wp_the_query->posts被填充赋值
	/*对我来说，实在是太过艰深晦涩；但个中巧妙又能极大提高我的能力。且留待今后慢慢消化罢。*/
	/**
	 * Parse request to find correct WordPress query.
	 *
	 * Sets up the query variables based on the request. There are also many
	 * filters and actions that can be used to further manipulate the result.
	 *
	 * @since 2.0.0
	 *
	 * @param array|string $extra_query_vars Set the extra query variables.
	 */
	function parse_request($extra_query_vars = '') {
		global $wp_rewrite;

		if ( ! apply_filters( 'do_parse_request', true, $this, $extra_query_vars ) )
			return;

		$this->query_vars = array();
		$post_type_query_vars = array();

		if ( is_array($extra_query_vars) )
			$this->extra_query_vars = & $extra_query_vars;
		else if (! empty($extra_query_vars))
			parse_str($extra_query_vars, $this->extra_query_vars);

		// Process PATH_INFO, REQUEST_URI, and 404 for permalinks.

		// Fetch the rewrite rules.
		$rewrite = $wp_rewrite->wp_rewrite_rules();

		if ( ! empty($rewrite) ) {
			// If we match a rewrite rule, this will be cleared.
			$error = '404';
			$this->did_permalink = true;

			$pathinfo = isset( $_SERVER['PATH_INFO'] ) ? $_SERVER['PATH_INFO'] : '';//PATH_INFO是.php与之后的query之间的路径部分。http://www.cnblogs.com/xiaochaohuashengmi/archive/2011/09/13/2175071.html
			list( $pathinfo ) = explode( '?', $pathinfo );
			$pathinfo = str_replace( "%", "%25", $pathinfo );

			list( $req_uri ) = explode( '?', $_SERVER['REQUEST_URI'] );//REQUEST_URI大致就是root/之后的所有东西
			$self = $_SERVER['PHP_SELF'];
			$home_path = trim( parse_url( home_url(), PHP_URL_PATH ), '/' );

			// Trim path info from the end and the leading home path from the
			// front. For path info requests, this leaves us with the requesting
			// filename, if any. For 404 requests, this leaves us with the
			// requested permalink.
			$req_uri = str_replace($pathinfo, '', $req_uri);//将$req_uri中的$pathinfo全部删除
			$req_uri = trim($req_uri, '/');//继续将$req_uri后面的'/'删除
			$req_uri = preg_replace("|^$home_path|i", '', $req_uri);//这里彻底搞晕了我：这个正则表达式到底是什么意思啊！
			$req_uri = trim($req_uri, '/');
			$pathinfo = trim($pathinfo, '/');
			$pathinfo = preg_replace("|^$home_path|i", '', $pathinfo);
			$pathinfo = trim($pathinfo, '/');
			$self = trim($self, '/');
			$self = preg_replace("|^$home_path|i", '', $self);
			$self = trim($self, '/');

			// The requested permalink is in $pathinfo for path info requests and
			//  $req_uri for other requests.
			if ( ! empty($pathinfo) && !preg_match('|^.*' . $wp_rewrite->index . '$|', $pathinfo) ) {
				$request = $pathinfo;
			} else {
				// If the request uri is the index, blank it out so that we don't try to match it against a rule.
				if ( $req_uri == $wp_rewrite->index )
					$req_uri = '';
				$request = $req_uri;
			}

			$this->request = $request;

			// Look for matches.
			$request_match = $request;
			if ( empty( $request_match ) ) {
				// An empty request could only match against ^$ regex
				if ( isset( $rewrite['$'] ) ) {
					$this->matched_rule = '$';
					$query = $rewrite['$'];
					$matches = array('');
				}
			} else {
				foreach ( (array) $rewrite as $match => $query ) {
					// If the requesting file is the anchor of the match, prepend it to the path info.
					if ( ! empty($req_uri) && strpos($match, $req_uri) === 0 && $req_uri != $request )
						$request_match = $req_uri . '/' . $request;

					if ( preg_match("#^$match#", $request_match, $matches) ||
						preg_match("#^$match#", urldecode($request_match), $matches) ) {

						if ( $wp_rewrite->use_verbose_page_rules && preg_match( '/pagename=\$matches\[([0-9]+)\]/', $query, $varmatch ) ) {
							// this is a verbose page match, lets check to be sure about it
							if ( ! get_page_by_path( $matches[ $varmatch[1] ] ) )
						 		continue;
						}

						// Got a match.
						$this->matched_rule = $match;
						break;
					}
				}
			}

			if ( isset( $this->matched_rule ) ) {
				// Trim the query of everything up to the '?'.
				$query = preg_replace("!^.+\?!", '', $query);

				// Substitute the substring matches into the query.
				$query = addslashes(WP_MatchesMapRegex::apply($query, $matches));

				$this->matched_query = $query;

				// Parse the query.
				parse_str($query, $perma_query_vars);

				// If we're processing a 404 request, clear the error var since we found something.
				if ( '404' == $error )
					unset( $error, $_GET['error'] );
			}

			// If req_uri is empty or if it is a request for ourself, unset error.
			if ( empty($request) || $req_uri == $self || strpos($_SERVER['PHP_SELF'], 'wp-admin/') !== false ) {
				unset( $error, $_GET['error'] );

				if ( isset($perma_query_vars) && strpos($_SERVER['PHP_SELF'], 'wp-admin/') !== false )
					unset( $perma_query_vars );

				$this->did_permalink = false;
			}
		}

		$this->public_query_vars = apply_filters('query_vars', $this->public_query_vars);

		foreach ( get_post_types( array(), 'objects' ) as $post_type => $t )
			if ( $t->query_var )
				$post_type_query_vars[$t->query_var] = $post_type;

		foreach ( $this->public_query_vars as $wpvar ) {
			if ( isset( $this->extra_query_vars[$wpvar] ) )
				$this->query_vars[$wpvar] = $this->extra_query_vars[$wpvar];
			elseif ( isset( $_POST[$wpvar] ) )
				$this->query_vars[$wpvar] = $_POST[$wpvar];
			elseif ( isset( $_GET[$wpvar] ) )
				$this->query_vars[$wpvar] = $_GET[$wpvar];
			elseif ( isset( $perma_query_vars[$wpvar] ) )
				$this->query_vars[$wpvar] = $perma_query_vars[$wpvar];

			if ( !empty( $this->query_vars[$wpvar] ) ) {
				if ( ! is_array( $this->query_vars[$wpvar] ) ) {
					$this->query_vars[$wpvar] = (string) $this->query_vars[$wpvar];
				} else {
					foreach ( $this->query_vars[$wpvar] as $vkey => $v ) {
						if ( !is_object( $v ) ) {
							$this->query_vars[$wpvar][$vkey] = (string) $v;
						}
					}
				}

				if ( isset($post_type_query_vars[$wpvar] ) ) {
					$this->query_vars['post_type'] = $post_type_query_vars[$wpvar];
					$this->query_vars['name'] = $this->query_vars[$wpvar];
				}
			}
		}

		// Convert urldecoded spaces back into +
		foreach ( get_taxonomies( array() , 'objects' ) as $taxonomy => $t )
			if ( $t->query_var && isset( $this->query_vars[$t->query_var] ) )
				$this->query_vars[$t->query_var] = str_replace( ' ', '+', $this->query_vars[$t->query_var] );

		// Limit publicly queried post_types to those that are publicly_queryable
		if ( isset( $this->query_vars['post_type']) ) {
			$queryable_post_types = get_post_types( array('publicly_queryable' => true) );
			if ( ! is_array( $this->query_vars['post_type'] ) ) {
				if ( ! in_array( $this->query_vars['post_type'], $queryable_post_types ) )
					unset( $this->query_vars['post_type'] );
			} else {
				$this->query_vars['post_type'] = array_intersect( $this->query_vars['post_type'], $queryable_post_types );
			}
		}

		foreach ( (array) $this->private_query_vars as $var) {
			if ( isset($this->extra_query_vars[$var]) )
				$this->query_vars[$var] = $this->extra_query_vars[$var];
		}

		if ( isset($error) )
			$this->query_vars['error'] = $error;

		$this->query_vars = apply_filters('request', $this->query_vars);

		do_action_ref_array('parse_request', array(&$this));
	}
