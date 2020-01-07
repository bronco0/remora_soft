document.addEventListener("DOMContentLoaded", function() {

  /* Display zones */
  let ordres = new Map();
  ordres.set('C','Confort');
  ordres.set('1','Confort -1');
  ordres.set('2','Confort -2');
  ordres.set('E','Éco');
  ordres.set('H','Hors gel');
  ordres.set('A','Arrêt');
    

  var httpRequest = new XMLHttpRequest();
  httpRequest.onreadystatechange = function(data) {
    if (data.target.readyState == 4) {
      createZone(JSON.parse(data.target.response))
    }
  };
  httpRequest.open("GET", '/fp');
  httpRequest.setRequestHeader(
    "Content-Type",
    "application/json",
  );
    httpRequest.send(null);
    

  function createZone(data) {
    var root_div = document.getElementById('remora-zone');

    for (let [key,value] of Object.entries(data)) {
      //console.log(`${key}: ${value}`);
      var div_cell = document.createElement('div');
      div_cell.className = "mdl-cell mdl-cell--4-col mdl-cell--6-col-tablet mdl-cell--4-col-phone";
      var div_zone = document.createElement('div');
      div_zone.className = "remora-zone mdl-card mdl-shadow--2dp";
      div_cell.appendChild(div_zone);
  
      var div_card_title = document.createElement('div');
      div_card_title.className = "mdl-card__title";
      var div_card_title_grid = document.createElement('div');
      div_card_title_grid.className = "mdl-grid";
  
      var div_card_title_cell_text = document.createElement('div');
      div_card_title_cell_text.className = "mdl-cell mdl-cell--4-col mdl-cell--1-col-phone";
      var h2_card_title = document.createElement('h2');
      h2_card_title.className = "mdl-card__title-text";
      var content = document.createTextNode(key);
      h2_card_title.appendChild(content);
      div_card_title_cell_text.appendChild(h2_card_title);
  
      var div_card_title_spacer = document.createElement('div');
      div_card_title_spacer.className = "mdl-layout-spacer";
  
      var div_card_title_cell_icon = document.createElement('div');
      div_card_title_cell_icon.className = "mdl-cell mdl-cell--4-col mdl-cell--1-col-phone";
      var span_icon_title = document.createElement('span');
      span_icon_title.className = "icon";
      var i_icon_jeedom = document.createElement('i');
      i_icon_jeedom.className = "icon jeedom-pilote-" + ordres.get(value).toLowerCase().normalize("NFD").replace(/[\u0300-\u036f]/g, "").replace(' ', '');
      span_icon_title.appendChild(i_icon_jeedom);
      div_card_title_cell_icon.appendChild(span_icon_title);
  
      div_card_title_grid.appendChild(div_card_title_cell_text);
      div_card_title_grid.appendChild(div_card_title_spacer);
      div_card_title_grid.appendChild(div_card_title_cell_icon);
  
      div_card_title.appendChild(div_card_title_grid);
  
      div_zone.appendChild(div_card_title);
  
      var div_card_actions = document.createElement('div');
      div_card_actions.className = "mdl-card__actions mdl-card--border";
      for (let [k,ordre] of ordres) {
        var button = document.createElement('a');
        button.className = "mdl-button mdl-button--colored mdl-js-button mdl-js-ripple-effect" + ((value == k) ? " mdl-button--raised" : "");
        button.href = "#";
        var content = document.createTextNode(ordre);
        button.appendChild(content);
        div_card_actions.appendChild(button);
      };
      div_zone.appendChild(div_card_actions);
      div_cell.appendChild(div_zone);
      root_div.appendChild(div_cell);
    }

    let elementsArray = root_div.querySelectorAll("a");
    //console.log(elementsArray)
    elementsArray.forEach(function(elem) {
        elem.addEventListener("click", function(e) {
            console.log(e.target);
            console.log(this);
        });
    });
  }
});