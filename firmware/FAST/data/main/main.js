jQuery.each( [ "put", "del" ], function( i, method ) {
  jQuery[ method ] = function( url, data, callback, type ) {
    if ( jQuery.isFunction( data ) ) {
      type = type || callback;
      callback = data;
      data = undefined;
    }

    return jQuery.ajax({
      type: method,
      url: url,
      data: data,
      success: callback,
      dataType: type
    });
  };
});

function long2ip(ip){
  return [ip >>> 0 & 0xFF, ip >>> 8 & 0xFF, ip >>> 16 & 0xFF, ip >>> 24 & 0xFF].join('.')
}

function updateStatus(status){
  $('span#info-status').text(status);
  $('#log-area').prepend($('<p>').text(Date().match(/.+(\d\d:\d\d:\d\d).+/)[1]+': '+status));
}
function load(){
  $.getJSON('/info',{},function(data) {
    const www_auth_method_map = ["basic", "digest"];
    // send buttons settings id list
    $('#send tbody').empty();
    $('#input-id').empty();
    $('#input-id').append($('<option>').val(-1).text("-select-"));
    // informations
    $('span#info-version').text(data["version"]);
    $('span#info-hostname').text(data["hostname"]);
    $('span#info-ssid').text(data["ssid"]);
    var local_ip = data["local_ip"];
    var subnetmask = data["subnetmask"];
    var gateway = data["gateway"];
    $('span#info-local_ip').text(long2ip(local_ip));
    $('#input-local_ip').val(long2ip(local_ip));
    $('#input-subnetmask').val(long2ip(subnetmask));
    $('#input-gateway').val(long2ip(gateway));
    $('#input-www_auth_method').val(www_auth_method_map[data["www_auth_method"]]);
    $('#input-www_username').val(data["www_username"]);
    $('#input-www_password').val(data["www_password"]);
  }).done(function(){
    $('#main').show();
  });
  $.getJSON('/indicator',{},function(data) {
    // informations
    $('output#indicator-red').val(data["red"]);
    $('output#indicator-green').val(data["green"]);
    $('output#indicator-blue').val(data["blue"]);
    $('output#indicator-interval').val(data["interval"]);
    $('input#indicator-flash').prop('checked', data["flash"]);
  }).done(function(){
    $('#main').show();
  });
  $.getJSON('/beep',{},function(data) {
    // informations
    $('output#beep-interval').val(data["interval"]);
    $('input#beep-repeat').prop('checked', data["repeat"]);
  }).done(function(){
    $('#main').show();
  });
}

function indicator(e){
  var action = $(e.target).val();
  switch(action){
    case "on":
    $.put('/indicator',JSON.stringify({
      red: $('#indicator-red').val(),
      green: $('#indicator-green').val(),
      blue: $('#indicator-blue').val(),
      interval: $('#indicator-interval').val(),
      flash: $('#indicator-flash').prop("checked")
    })
    ).done(function(res){
      updateStatus(JSON.stringify(res));
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
    case "off":
    $.put('/indicator',JSON.stringify({
      red: 0,
      green: 0,
      blue: 0,
      interval: $('#indicator-interval').val(),
      flash: false
    })
    ).done(function(res){
      updateStatus(JSON.stringify(res));
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
  }
}
$('#indicator button').click(indicator);

function beep(e){
  var action = $(e.target).val();
  switch(action){
    case "on":
    $.put('/beep',JSON.stringify({
      isOn: true,
      interval: $('#beep-interval').val(),
      repeat: $('#beep-repeat').prop("checked")
    })
    ).done(function(res){
      updateStatus(JSON.stringify(res));
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
    case "off":
    $.put('/beep',JSON.stringify({
      isOn: false,
      interval: $('#beep-interval').val(),
      repeat: false
    })
    ).done(function(res){
      updateStatus(JSON.stringify(res));
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
  }
}
$('#beep button').click(beep);

/* manage */
$('#manage select[name="action"]').change(function(){
  var action = $(this).val();
  $('#form-submit label').text("")
  $('#input-time').val((new Date((new Date()).getTime()+9*60*60*1000)).toISOString().substring(0,17)+"00");
  switch(action){
    case "change-ip":
    $('#form-id').hide();
    $('#form-position').hide();
    $('#form-name').hide();
    $('#form-file').hide();
    $('#form-time').hide();
    $('#form-ipaddress').show();
    $('#form-settings').hide();
    break;
    case "disconnect-wifi":
    $('#form-id').hide();
    $('#form-position').hide();
    $('#form-name').hide();
    $('#form-file').hide();
    $('#form-time').hide();
    $('#form-ipaddress').hide();
    $('#form-settings').hide();
    break;
    case "settings":
    $('#form-id').hide();
    $('#form-position').hide();
    $('#form-name').hide();
    $('#form-file').hide();
    $('#form-time').hide();
    $('#form-ipaddress').hide();
    $('#form-settings').show();
    break;
  }
});
function manage(){
  var action = $('#input-action').val();
  switch(action){
    case "disconnect-wifi":
    if(!confirm('Are you sure to disconnect this WiFi?'))return;
    $.post('/wifi/disconnect');
    $('#main').hide();
    break;
    case "change-ip":
    var local_ip = $('#input-local_ip').val();
    var subnetmask = $('#input-subnetmask').val();
    var gateway = $('#input-gateway').val();
    if(!local_ip.match(/^\d{1,3}(\.\d{1,3}){3}$/))return $('#form-submit label').text("Invalid IP address");
    if(!subnetmask.match(/^\d{1,3}(\.\d{1,3}){3}$/))return $('#form-submit label').text("Invalid Subnet Mask");
    if(!gateway.match(/^\d{1,3}(\.\d{1,3}){3}$/))return $('#form-submit label').text("Invalid Gateway IP");
    if(!confirm('Are you sure to change ip address?')) return;
    $('#main').hide();
    $.post('/wifi/change-ip',{
      local_ip: $('#input-local_ip').val(),
      subnetmask: $('#input-subnetmask').val(),
      gateway: $('#input-gateway').val()
    }).done(function(res){
      updateStatus(res);
      $('#main').show();
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
    case "settings":
    var www_auth_method = $('#input-www_auth_method').val();
    var www_username = $('#input-www_username').val();
    var www_password = $('#input-www_password').val();
    if(!www_auth_method.match(/^(digest|basic)$/))return $('#form-submit label').text("Invalid Authentication Method");
    // if(!www_username.match(/^\d{1,3}(\.\d{1,3}){3}$/))return $('#form-submit label').text("Invalid Subnet Mask");
    // if(!www_password.match(/^\d{1,3}(\.\d{1,3}){3}$/))return $('#form-submit label').text("Invalid Gateway IP");
    if(!confirm('Are you sure to change settings?')) return;
    $.put('/settings',JSON.stringify({
      www_auth_method: www_auth_method,
      www_username: www_username,
      www_password: www_password
    })
    ).done(function(res){
      updateStatus(JSON.stringify(res));
      location.href = "/";
    }).fail(function(){
      location.href = "http://"+$('#input-local_ip').val();
    })
    break;
  }
  $('#form-submit label').text("")
}
$('#manage button').click(manage);
$('#manage input').keypress(function(e){
  if(e.which == 13){
    manage();
  }
});

/* init */
load();
updateStatus("Loading Succeeded.");
