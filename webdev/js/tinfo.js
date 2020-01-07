document.addEventListener("DOMContentLoaded", function() {
  var httpRequest = new XMLHttpRequest();
  httpRequest.onreadystatechange = function(data) {
    if (data.target.readyState == 4) {
      _refreshTinfo(JSON.parse(data.target.response))
      _refreshTinfoProgressBar(JSON.parse(data.target.response));
    }
  };
  httpRequest.open("GET", '/tinfo.json');
  httpRequest.setRequestHeader(
    "Content-Type",
    "application/json",
  );
  httpRequest.send(null);
});

function refreshTinfo() {
  var root_div = document.getElementById('remora-tinfo');
  
  if (root_div.style.display == 'flex') {
    timer_tinfo = setInterval(function(){
      var httpRequest = new XMLHttpRequest();
      httpRequest.onreadystatechange = function(data) {
        if (data.target.readyState == 4) {
          _refreshTinfo(JSON.parse(data.target.response));
          _refreshTinfoProgressBar(JSON.parse(data.target.response));
        }
      };
      httpRequest.open("GET", '/tinfo.json');
      httpRequest.setRequestHeader(
        "Content-Type",
        "application/json",
      );
      httpRequest.send(null);
    }, 1000);
  }
}

function _refreshTinfo(data) {
  var root_div = document.getElementById('remora-tinfo');
  var table = root_div.querySelector('table'); 
  var old_tbody = root_div.querySelector('table > tbody');
  var tbody = document.createElement('tbody');

  for (let [key,value] of Object.entries(data)) {
    var tr = document.createElement('tr');
    var td_name = document.createElement('td');
    td_name.className = "mdl-data-table__cell--non-numeric";
    var td_name_content = document.createTextNode(key);
    td_name.appendChild(td_name_content);
    tr.appendChild(td_name);

    for (let [td_key,td_value] of Object.entries(value)) {
      var td = document.createElement('td');
      if (td_key == 'ck') {
        td.className = "mdl-data-table__cell--center";
      }
      else {
        td.className = "mdl-data-table__cell--non-numeric";
      }
      var td_content = document.createTextNode(td_value);
      td.appendChild(td_content);
      tr.appendChild(td);
    }
    tbody.appendChild(tr);
  }
  table.replaceChild(tbody, old_tbody);
}

function _refreshTinfoProgressBar(data) {
  var isousc  = data.ISOUSC.va;
  var iinst = data.IINST.va;
  var ratio = iinst*100/isousc;

  document.querySelector('#prog1').addEventListener('mdl-componentupgraded', function(){
    this.MaterialProgress.setProgress(0);
  });

  document.querySelector("#remora-tinfo h6").textContent = parseInt(iinst, 10) + "A / " + parseInt(isousc, 10) + "A";
}7