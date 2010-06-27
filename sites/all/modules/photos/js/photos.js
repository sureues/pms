// $Id: photos.js,v 1.1.2.2 2009/03/11 02:38:31 eastcn Exp $
Drupal.behaviors.photos = function (context) {
  $('.photos_block_sub_open').click(function(){
    v = $(this).parent().siblings('.photos_block_sub_body');
    v.toggle(300);
    ck = $(this);
    $(this).toggleClass('photos_block_sub_open_c');
    if(url = $(this).attr('alt')){
      $.getJSON(url, function(json){
          var h = '';
          for(i = 0; i < json.count; i++){
            h += '<a href="' + json[i].link + '"><img src="' +json[i].url+ '" title="' +json[i].filename+ '"/></a>';
          }
          v.html(h);
          ck.attr('alt', '');
          v.removeClass('photos_block_sub_body_load');
        }
      );
    }
  }).hover(function(){
    $(this).addClass('photos_block_sub_open_hover');
  },function(){
    $(this).removeClass('photos_block_sub_open_hover');
  });
  $('#del_check').click(function(){
    if(this.checked){
      $(".edit-del-all input[@type='checkbox']").attr('checked', true);
      $('.edit-del-all').parents('tr').addClass('selected');
    }else{
      $(".edit-del-all input[@type='checkbox']").attr('checked', false);
      $('.edit-del-all').parents('tr').removeClass('selected');
      $('textarea, input, select', $('.edit-del-all').parents('tr')).not(this).attr('disabled', false).removeClass('photos_check');
    }
  });
  $('.photos_imagehtml').hover(function(){
    $(this).children('.photos_imagehtml_thickbox').css('display', 'block');
  },function(){
    $(this).children('.photos_imagehtml_thickbox').css('display', 'none');
  });
  $('.edit-del-all input').click(function(){
    if(this.checked){
      $(this).parents('tr').addClass('selected');
    }else{
      $(this).parents('tr').removeClass('selected');
    }
  });
	$('a.photos-vote').click(function(){
    var $$ = $(this);
    u = $(this).attr('href');
		url = u.split('destination=');
		var c = $(this).hasClass('photos-vote-up');
		$.getJSON(url[0], function(json){
      $$.addClass('photos-vote-load');
      if(json.count > 0){
    		if(c) {
    			$$.siblings('.photos-vote-down').removeClass('photos-vote-down-x');
          $$.addClass('photos-vote-up-x').unbind('click');
    		}else {
    			$$.siblings('.photos-vote-up').removeClass('photos-vote-up-x');
          $$.addClass('photos-vote-down-x').unbind('click');
    		}
  		  $('.photos-vote-sum_' + $$.attr('alt')).text(json.sum + ' / ' +json.count);
      }else{
        alert(Drupal.t('Operation failed.'));
      }
      $$.removeClass('photos-vote-load').attr('href', '#');
		});
		return false;
	});
  $('#photos_share li').hover(function(){
    $(this).addClass('photos_share_hover');
  },function(){
    $(this).removeClass('photos_share_hover');
  })
  $('#photos_share_ul li select[@class="photos_share_select_val"]').change(function(){
    $(this).parents('li')[ $(this).val() != 0 ? 'addClass' : 'removeClass' ]('photos_share_selected');
  });
  $('a.photos_share_copy').click(function(){
    var alt = $(this).attr('alt');
    var text = '';
    $('#photos_share_ul li select[@class="photos_share_select_val"]').each(function(){
      if($(this).val() != 0){
        if(alt == 'ubb'){
          text += '[photo=image]id=' + $(this).attr('alt') + '|label=' + $('option:selected', this).text() + '[/photo]\n';
        }else{
          text += '<a href="' + $(this).parents('li').attr('alt') + '"><img src="' + $('option:selected', this).val() + '" title="' + $(this).parents('li').attr('title') + '"/></a>\n';
        }
      }
    });
    $('#photos_share_textarea').val(text).select();
    $('.photos_share_textarea').show(300);
    return false;
  });
	$('input[@class="image-quote-link"], .photos_share_textarea').click(function(){
		$(this).select();
	}); 
	$('input[@class="photos-p"]').change(function() {
		i = 0;
		$('#edit-insert-wrapper').show(500);
		$("input[@class='photos-p'],input[@class='photos-pp']").each(function() {
			if($(this).attr("checked")) {
				t = $(this).val().split('&&&');
				img = '<a class="image-quote" href="http://' + window.location.host +'/photos/image/' + t[2] + '">' + '<img alt="' + t[1] + '" src="' + t[0] + '" /></a>\n';
				inval = $('#edit-insert').val();
				$('#edit-insert').val(inval + img).select();
				$(this).parents('.photos-quote').hide(600,function(){
					$(this).remove();
				});
				i++;
			}	
		})
		if(i > 0) {
			$('.photo-msg').text(Drupal.t('Please copy the above code.')).show(500);
		}
		return false;
	})
  var atext = [Drupal.t('Save'), Drupal.t('Cancel'), Drupal.t('Being updated...'), Drupal.t('Click to edit')];
  $('.jQueryeditable_edit_filename, .jQueryeditable_edit_des').hover(function(){
    $(this).addClass('photot_ajax_hover');
  },function(){
    $(this).removeClass('photot_ajax_hover');
  });
  $('.jQueryeditable_edit_filename').editable('/photos/image', {width: '50%', submit : atext[0], cancel : atext[1], indicator : atext[2], tooltip: atext[3], loadtype: 'POST'}, function(){
    return false;
  });
  $('.jQueryeditable_edit_des').editable('/photos/image', {height: 140, submit : atext[0], cancel : atext[1], indicator : atext[2],tooltip: atext[3], type: 'textarea', loadtype: 'POST'}, function(){
    return false;
  });
  $('.jQueryeditable_edit_delete').click(function(){ 
    d = $(this).attr('alt');
    $(this).addClass('photos_ajax_del_img');
    $.get($(this).attr('href'), {go: 1}, function(v){
      if(v == 1){
        $('#'+d).remove();
      }else{
        alert(Drupal.t('Delete failure'));
      }
    });
    return false;
  }); 
};
function select_hide(t, dom){
  switch(t){
    case '0':
    case '1':
      $('.photos-form-count, .photos-form-slide', dom).hide(300);
    break;
    case '2':
      $('.photos-form-count', dom).show(300);
      $('.photos-form-slide', dom).hide(300);
    break;
    case '3':
      $('.photos-form-slide', dom).show(300);
      $('.photos-form-count', dom).hide(300);
    break;
  }
}