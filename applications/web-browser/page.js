function replace_all_rel_by_abs(base_url, html) {
	var att = "[^-a-z0-9:._]";

	var entityEnd = "(?:;|(?!\\d))";
	var ents = {
		" ": "(?:\\s|&nbsp;?|&#0*32" + entityEnd + "|&#x0*20" + entityEnd + ")",
		"(": "(?:\\(|&#0*40" + entityEnd + "|&#x0*28" + entityEnd + ")",
		")": "(?:\\)|&#0*41" + entityEnd + "|&#x0*29" + entityEnd + ")",
		".": "(?:\\.|&#0*46" + entityEnd + "|&#x0*2e" + entityEnd + ")",
	};
	var charMap = {};
	var s = ents[" "] + "*";
	var any = "(?:[^>\"']*(?:\"[^\"]*\"|'[^']*'))*?[^>]*";
	function ae(string) {
		var all_chars_lowercase = string.toLowerCase();
		if (ents[string]) return ents[string];
		var all_chars_uppercase = string.toUpperCase();
		var RE_res = "";
		for (var i = 0; i < string.length; i++) {
			var char_lowercase = all_chars_lowercase.charAt(i);
			if (charMap[char_lowercase]) {
				RE_res += charMap[char_lowercase];
				continue;
			}
			var char_uppercase = all_chars_uppercase.charAt(i);
			var RE_sub = [char_lowercase];
			RE_sub.push("&#0*" + char_lowercase.charCodeAt(0) + entityEnd);
			RE_sub.push(
				"&#x0*" + char_lowercase.charCodeAt(0).toString(16) + entityEnd
			);
			if (char_lowercase != char_uppercase) {
				/* Note: RE ignorecase flag has already been activated */
				RE_sub.push("&#0*" + char_uppercase.charCodeAt(0) + entityEnd);
				RE_sub.push(
					"&#x0*" +
						char_uppercase.charCodeAt(0).toString(16) +
						entityEnd
				);
			}
			RE_sub = "(?:" + RE_sub.join("|") + ")";
			RE_res += charMap[char_lowercase] = RE_sub;
		}
		return (ents[string] = RE_res);
	}
	function by(match, group1, group2, group3) {
		return group1 + rel_to_abs(base_url, group2) + group3;
	}
	var slashRE = new RegExp(ae("/"), "g");
	var dotRE = new RegExp(ae("."), "g");
	function by2(match, group1, group2, group3) {
		/*Note that this function can also be used to remove links:
		 * return group1 + "javascript://" + group3; */
		group2 = group2.replace(slashRE, "/").replace(dotRE, ".");
		return group1 + rel_to_abs(base_url, group2) + group3;
	}
	function cr(selector, attribute, marker, delimiter, end) {
		if (typeof selector == "string") selector = new RegExp(selector, "gi");
		attribute = att + attribute;
		marker = typeof marker == "string" ? marker : "\\s*=\\s*";
		delimiter = typeof delimiter == "string" ? delimiter : "";
		end = typeof end == "string" ? "?)(" + end : ")(";
		var re1 = new RegExp(
			"(" + attribute + marker + '")([^"' + delimiter + "]+" + end + ")",
			"gi"
		);
		var re2 = new RegExp(
			"(" + attribute + marker + "')([^'" + delimiter + "]+" + end + ")",
			"gi"
		);
		var re3 = new RegExp(
			"(" +
				attribute +
				marker +
				")([^\"'][^\\s>" +
				delimiter +
				"]*" +
				end +
				")",
			"gi"
		);
		html = html.replace(selector, function (match) {
			return match.replace(re1, by).replace(re2, by).replace(re3, by);
		});
	}
	function cri(selector, attribute, front, flags, delimiter, end) {
		if (typeof selector == "string") selector = new RegExp(selector, "gi");
		attribute = att + attribute;
		flags = typeof flags == "string" ? flags : "gi";
		var re1 = new RegExp("(" + attribute + '\\s*=\\s*")([^"]*)', "gi");
		var re2 = new RegExp("(" + attribute + "\\s*=\\s*')([^']+)", "gi");
		var at1 = new RegExp("(" + front + ')([^"]+)(")', flags);
		var at2 = new RegExp("(" + front + ")([^']+)(')", flags);
		if (typeof delimiter == "string") {
			end = typeof end == "string" ? end : "";
			var at3 = new RegExp(
				"(" +
					front +
					")([^\"'][^" +
					delimiter +
					"]*" +
					(end ? "?)(" + end + ")" : ")()"),
				flags
			);
			var handleAttr = function (match, g1, g2) {
				return (
					g1 +
					g2.replace(at1, by2).replace(at2, by2).replace(at3, by2)
				);
			};
		} else {
			var handleAttr = function (match, g1, g2) {
				return g1 + g2.replace(at1, by2).replace(at2, by2);
			};
		}
		html = html.replace(selector, function (match) {
			return match.replace(re1, handleAttr).replace(re2, handleAttr);
		});
	}
	// <meta http-equiv=refresh content="  ; url= " >
	cri(
		"<meta" +
			any +
			att +
			'http-equiv\\s*=\\s*(?:"' +
			ae("refresh") +
			'"' +
			any +
			">|'" +
			ae("refresh") +
			"'" +
			any +
			">|" +
			ae("refresh") +
			"(?:" +
			ae(" ") +
			any +
			">|>))",
		"content",
		ae("url") + s + ae("=") + s,
		"i"
	);
	// Linked elements
	cr("<" + any + att + "href\\s*=" + any + ">", "href");
	// Embedded elements
	cr("<" + any + att + "src\\s*=" + any + ">", "src");
	// <object data= >
	cr("<object" + any + att + "data\\s*=" + any + ">", "data");
	// <applet codebase= >
	cr("<applet" + any + att + "codebase\\s*=" + any + ">", "codebase");
	/* <param name=movie value= >*/
	cr(
		"<param" +
			any +
			att +
			'name\\s*=\\s*(?:"' +
			ae("movie") +
			'"' +
			any +
			">|'" +
			ae("movie") +
			"'" +
			any +
			">|" +
			ae("movie") +
			"(?:" +
			ae(" ") +
			any +
			">|>))",
		"value"
	);
	// <style>
	cr(
		/<style[^>]*>(?:[^"']*(?:"[^"]*"|'[^']*'))*?[^'"]*(?:<\/style|$)/gi,
		"url",
		"\\s*\\(\\s*",
		"",
		"\\s*\\)"
	);
	// < style=" url(...) " >
	cri(
		"<" + any + att + "style\\s*=" + any + ">",
		"style",
		ae("url") + s + ae("(") + s,
		0,
		s + ae(")"),
		ae(")")
	);
	return html;
}
function rel_to_abs(base_url, url) {
	if (
		/^(https?|file|ftps?|mailto|javascript|data:image\/[^;]{2,9};):/i.test(
			url
		)
	) {
		return url; //Url is already absolute
	}
	base_url = base_url.match(/^(.+)\/?(?:#.+)?$/)[0] + "/";
	if (url.substring(0, 2) === "//") {
		return "https:" + url;
	} else if (url.charAt(0) === "/") {
		var matches = base_url.match(/^https?\:\/\/([^\/?#]+)(?:[\/?#]|$)/i);
		var domain = matches && matches[1];
		return "https://" + domain + url;
	} else if (url.substring(0, 2) === "./") {
		url = "." + url;
	} else if (/^\s*$/.test(url)) {
		return ""; //Empty = Return nothing
	} else {
		url = "../" + url;
	}
	url = base_url + url;
	var i = 0;
	while (/\/\.\.\//.test((url = url.replace(/[^\/]+\/+\.\.\//g, "")))) {}
	/* Escape certain characters to prevent XSS */
	return url
		.replace(/\.$/, "")
		.replace(/\/\./g, "")
		.replace(/"/g, "%22")
		.replace(/'/g, "%27")
		.replace(/</g, "%3C")
		.replace(/>/g, "%3E");
}
