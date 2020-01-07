document.addEventListener("DOMContentLoaded", function() {
  let relais_ico = ['off', 'on'];
  let relais_fct = ['Arrêt', 'Auto','Marche forcée'];

  var httpRequest = new XMLHttpRequest();
  httpRequest.onreadystatechange = function(data) {
    if (data.target.readyState == 4) {
      createRelais(JSON.parse(data.target.response))
    }
  };
  httpRequest.open("GET", '/relais');
  httpRequest.setRequestHeader(
    "Content-Type",
    "application/json",
  );
    httpRequest.send(null);

  function createRelais(data) {
    //console.log(data);
    var root_div = document.getElementById('remora-relais');

    // change icon
    root_div.querySelector('i.icon').className += (" jeedom-relais-" + relais_ico[data.relais]);
    
    // raise button
    root_div.querySelectorAll('a.mdl-button').forEach(function(button) {
      button.className += ((relais_fct[data.fnct_relais] == button.textContent) ? " mdl-button--raised" : "")
    })
    
    let elementsArray = root_div.querySelectorAll("a");
    elementsArray.forEach(function(elem) {
        elem.addEventListener("click", function(e) {
            console.log(e.target);
        });
    });
  }
});