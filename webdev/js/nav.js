var timer_sys   = undefined,
    timer_tinfo = undefined;

document.addEventListener("DOMContentLoaded", function() {
  var root_nav = document.querySelector('nav');
  let elementsArray = root_nav.querySelectorAll("a.mdl-navigation__link");
  elementsArray.forEach(function(elem) {
    elem.addEventListener("click", function(e) {
      var views = document.querySelectorAll('main.mdl-layout__content > div');
      views.forEach(function(view) {
        if (view.style.display == 'flex') {
          view.style.display = 'none';
        }
        if ("remora-" + e.target.getAttribute('data-nav') == view.getAttribute('id')) {
          view.style.display = 'flex';
          //console.log(timer_tinfo);
          if (e.target.getAttribute('data-nav') == 'tinfo' && timer_tinfo === undefined) {
            refreshTinfo();
          }
          else if (e.target.getAttribute('data-nav') != 'tinfo' && timer_tinfo !== undefined)
          {
            clearInterval(timer_tinfo);
            timer_tinfo = undefined;
          }
        }
      });
    });
  });
});