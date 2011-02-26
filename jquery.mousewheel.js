/*
  @library      jQuery Mousewheel Plugin
  @version      3.0.4
  @author       Brandon Aaron <brandon.aaron@gmail.com>
  @copyright    Copyright (c) 2010
  @website      http://brandonaaron.net/code/mousewheel/docs
  
  Licensed under the MIT License (LICENSE.txt).
  
  Thanks to: http://adomas.org/javascript-mouse-wheel/ for some pointers.
  Thanks to: Mathias Bank(http://www.mathias-bank.de) for a scope bug fix.
  Thanks to: Seamus Leahy for adding deltaX and deltaY
*/
(function(d){var e=['DOMMouseScroll','mousewheel'];d.event.special.mousewheel={setup:function(){if(this.addEventListener){for(var a=e.length;a;){this.addEventListener(e[--a],g,false)}}else{this.onmousewheel=g}},teardown:function(){if(this.removeEventListener){for(var a=e.length;a;){this.removeEventListener(e[--a],g,false)}}else{this.onmousewheel=null}}};d.fn.extend({mousewheel:function(a){return a?this.bind("mousewheel",a):this.trigger("mousewheel")},unmousewheel:function(a){return this.unbind("mousewheel",a)}});function g(a){var b=a||window.event,i=[].slice.call(arguments,1),c=0,j=true,h=0,f=0;a=d.event.fix(b);a.type="mousewheel";if(a.wheelDelta){c=a.wheelDelta/120}if(a.detail){c=-a.detail/3}f=c;if(b.axis!==undefined&&b.axis===b.HORIZONTAL_AXIS){f=0;h=-1*c}if(b.wheelDeltaY!==undefined){f=b.wheelDeltaY/120}if(b.wheelDeltaX!==undefined){h=-1*b.wheelDeltaX/120}i.unshift(a,c,h,f);return d.event.handle.apply(this,i)}})(jQuery);
