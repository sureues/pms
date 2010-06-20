// $Id: 

var doc = Drupal.document || {};

/*
 * $(document).ajaxSend(function(event, request, settings) { var $body =
 * $("body"); $body.addClass("disableDocument"); $body.click(function(e) {
 * window.event.returnValue = false; }); });
 * 
 * $(document).ajaxComplete(function(event, request, settings) { var $body =
 * $("body"); $body.removeClass("disableDocument"); $body.unbind("click"); });
 */

doc.ajax_request = function(path, data, successCallback) {
	$.ajax( {
		cache: false,
		url: Drupal.settings.basePath + 'document/' + path,
		dataType: 'text',
		data: data,
		error: function(request, status, error) {
			alert(status);
		},
		success: function(data, status, request) {
			successCallback(data, status, request);
		}
	});
}

doc.add_type = function() {
	var input = $("input[name=document_type]");
	var type = input.val();

	if (type.length == 0)
		return (false);

	doc.ajax_request('add_type', {
		type: type
	}, function(data, status, request) {
		if (data && data.length > 0) {
			if (data == parseInt(data)) {
				input.val('');
				$("select[name='document_types[]']")[0].options.add(new Option(type,
						data));
			} else {
				alert('Error: ' + data);
			}
		} else {
			alert('Unknown Error');
		}
	});

	return (false);
}

doc.delete_types = function() {
	var select = $("select[name='document_types[]']");
	var vals = select.val() || [];

	if (vals.length == 0)
		return (false);

	doc.ajax_request('delete_types', {
		ids: vals.join(',')
	}, function(data, status, request) {
		if (data && data.length > 0) {
			alert('Error: ' + data);
		} else {
			for (i = select[0].length - 1; i >= 0; i--) {
				if (select[0].options[i].selected) {
					select[0].remove(i);
				}
			}
		}
	});

	return (false);
}

doc.deleteDoc = function(img, cls) {
	var nid = img.getAttribute('nid');
	img = $(img);
	img.removeClass(cls);
	img.addClass('icon-loading');
	var opt = confirm('Really delete this document?');
	if (opt) {
		doc.ajax_request('delete_doc', {
			ids: nid
		}, function(data, status, request) {
			if (data && data.length > 0) {
				alert('Error: ' + data);
				img.removeClass('icon-loading');
				img.addClass(cls);
			} else {
				window.location = window.location;
			}
		});
	} else {
		img.removeClass('icon-loading');
		img.addClass(cls);
	}
}

doc.changeDocStatus = function(img, cls, publish) {
	var nid = img.getAttribute('nid');
	var status, message;
	if (publish) {
		status = Drupal.settings.document.STATUS_PUBLISHED;
		message = 'Publish this document?';
	} else {
		status = Drupal.settings.document.STATUS_UNPUBLISHED;
		message = 'Unpublish this document?';
	}

	img = $(img);
	img.removeClass(cls);
	img.addClass('icon-loading');
	var opt = confirm(message);
	if (opt) {
		doc.ajax_request('change_doc_status', {
			ids: nid,
			status: status
		}, function(data, status, request) {
			if (data && data.length > 0) {
				alert('Error: ' + data);
				img.removeClass('icon-loading');
				img.addClass(cls);
			} else {
				window.location = window.location;
			}
		});
	} else {
		img.removeClass('icon-loading');
		img.addClass(cls);
	}
}

doc.performSearch = function() {
	var searchText = $("input[name=search_text]").val();
	var searchFields = $("input:radio:checked[name=search_fields]").val();
	var searchYear = $("select[name=search_year]").val();
	var searchDocType = $("select[name=search_doctype]").val();

	var obj = {
		searchText: searchText,
		searchFields: searchFields,
		searchYear: searchYear,
		searchDocType: searchDocType
	};

	$("#document_loading").show();
	doc.ajax_request('search', {
		criteria: $.JSON.encode(obj)
	}, function(data, status, request) {
		$("#document_loading").hide();
		if (data && data.length > 0) {
			var div = $('#document_search_results');
			div.html(data);
		} else {
			alert('Error: ' + data);
		}
	});
	return (false);
}

doc.suppressSearchEnter = function(e) {
	var keycode;
	if (window.event) {
		keycode = window.event.keyCode;
	} else if (e) {
		keycode = e.which;
	} else {
		return (false);
	}

	if (keycode == 13) {
		doc.performSearch();
		return (false);
	} else {
		return (true);
	}
}

// ///////////////////////////////////////////////
/**
 * jQuery.JSON.encode( json-serializble ) Converts the given argument into a
 * JSON respresentation.
 * 
 * If an object has a "toJSON" function, that will be used to get the
 * representation. Non-integer/string keys are skipped in the object, as are
 * keys that point to a function.
 * 
 * json-serializble: The *thing* to be converted.
 */
jQuery.JSON = {
	encode: function(o) {
		if (typeof (JSON) == 'object' && JSON.stringify)
			return JSON.stringify(o);

		var type = typeof (o);

		if (o === null)
			return "null";

		if (type == "undefined")
			return undefined;

		if (type == "number" || type == "boolean")
			return o + "";

		if (type == "string")
			return this.quoteString(o);

		if (type == 'object') {
			if (typeof o.toJSON == "function")
				return this.encode(o.toJSON());

			if (o.constructor === Date) {
				var month = o.getUTCMonth() + 1;
				if (month < 10)
					month = '0' + month;

				var day = o.getUTCDate();
				if (day < 10)
					day = '0' + day;

				var year = o.getUTCFullYear();

				var hours = o.getUTCHours();
				if (hours < 10)
					hours = '0' + hours;

				var minutes = o.getUTCMinutes();
				if (minutes < 10)
					minutes = '0' + minutes;

				var seconds = o.getUTCSeconds();
				if (seconds < 10)
					seconds = '0' + seconds;

				var milli = o.getUTCMilliseconds();
				if (milli < 100)
					milli = '0' + milli;
				if (milli < 10)
					milli = '0' + milli;

				return '"' + year + '-' + month + '-' + day + 'T' + hours + ':'
						+ minutes + ':' + seconds + '.' + milli + 'Z"';
			}

			if (o.constructor === Array) {
				var ret = [];
				for ( var i = 0; i < o.length; i++)
					ret.push(this.encode(o[i]) || "null");

				return "[" + ret.join(",") + "]";
			}

			var pairs = [];
			for ( var k in o) {
				var name;
				var type = typeof k;

				if (type == "number")
					name = '"' + k + '"';
				else if (type == "string")
					name = this.quoteString(k);
				else
					continue; // skip non-string or number keys

				if (typeof o[k] == "function")
					continue; // skip pairs where the value is a function.

				var val = this.encode(o[k]);

				pairs.push(name + ":" + val);
			}

			return "{" + pairs.join(", ") + "}";
		}
	},

	/**
	 * jQuery.JSON.decode(src) Evaluates a given piece of json source.
	 */
	decode: function(src) {
		if (typeof (JSON) == 'object' && JSON.parse)
			return JSON.parse(src);
		return eval("(" + src + ")");
	},

	/**
	 * jQuery.JSON.decodeSecure(src) Evals JSON in a way that is *more* secure.
	 */
	decodeSecure: function(src) {
		if (typeof (JSON) == 'object' && JSON.parse)
			return JSON.parse(src);

		var filtered = src;
		filtered = filtered.replace(/\\["\\\/bfnrtu]/g, '@');
		filtered = filtered
				.replace(
						/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,
						']');
		filtered = filtered.replace(/(?:^|:|,)(?:\s*\[)+/g, '');

		if (/^[\],:{}\s]*$/.test(filtered))
			return eval("(" + src + ")");
		else
			throw new SyntaxError("Error parsing JSON, source is not valid.");
	},

	/**
	 * jQuery.JSON.quoteString(string) Returns a string-repr of a string, escaping
	 * quotes intelligently. Mostly a support function for JSON.encode.
	 * 
	 * Examples: >>> jQuery.JSON.quoteString("apple") "apple"
	 * 
	 * >>> jQuery.JSON.quoteString('"Where are we going?", she asked.') "\"Where
	 * are we going?\", she asked."
	 */
	quoteString: function(string) {
		if (string.match(this._escapeable)) {
			return '"' + string.replace(this._escapeable, function(a) {
				var c = this._meta[a];
				if (typeof c === 'string')
					return c;
				c = a.charCodeAt();
				return '\\u00' + Math.floor(c / 16).toString(16)
						+ (c % 16).toString(16);
			}) + '"';
		}
		return '"' + string + '"';
	},

	_escapeable: /["\\\x00-\x1f\x7f-\x9f]/g,

	_meta: {
		'\b': '\\b',
		'\t': '\\t',
		'\n': '\\n',
		'\f': '\\f',
		'\r': '\\r',
		'"': '\\"',
		'\\': '\\\\'
	}
}
