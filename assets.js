
var Module;

if (typeof Module === 'undefined') Module = eval('(function() { try { return Module || {} } catch(e) { return {} } })()');

if (!Module.expectedDataFileDownloads) {
  Module.expectedDataFileDownloads = 0;
  Module.finishedDataFileDownloads = 0;
}
Module.expectedDataFileDownloads++;
(function() {
 var loadPackage = function(metadata) {

    var PACKAGE_PATH;
    if (typeof window === 'object') {
      PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
    } else if (typeof location !== 'undefined') {
      // worker
      PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
    } else {
      throw 'using preloaded data can only be done on a web page or in a web worker';
    }
    var PACKAGE_NAME = 'assets';
    var REMOTE_PACKAGE_BASE = 'assets';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
  
      var REMOTE_PACKAGE_SIZE = 33757610;
      var PACKAGE_UUID = 'f35abc5c-bedf-4ed7-806a-3b79ca0c629c';
    
    function fetchRemotePackage(packageName, packageSize, callback, errback) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', packageName, true);
      xhr.responseType = 'arraybuffer';
      xhr.onprogress = function(event) {
        var url = packageName;
        var size = packageSize;
        if (event.total) size = event.total;
        if (event.loaded) {
          if (!xhr.addedTotal) {
            xhr.addedTotal = true;
            if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
            Module.dataFileDownloads[url] = {
              loaded: event.loaded,
              total: size
            };
          } else {
            Module.dataFileDownloads[url].loaded = event.loaded;
          }
          var total = 0;
          var loaded = 0;
          var num = 0;
          for (var download in Module.dataFileDownloads) {
          var data = Module.dataFileDownloads[download];
            total += data.total;
            loaded += data.loaded;
            num++;
          }
          total = Math.ceil(total * Module.expectedDataFileDownloads/num);
          if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
        } else if (!Module.dataFileDownloads) {
          if (Module['setStatus']) Module['setStatus']('Downloading data...');
        }
      };
      xhr.onload = function(event) {
        var packageData = xhr.response;
        callback(packageData);
      };
      xhr.send(null);
    };

    function handleError(error) {
      console.error('package error:', error);
    };
  
      var fetched = null, fetchedCallback = null;
      fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, function(data) {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);
    
  function runWithFS() {

    function assert(check, msg) {
      if (!check) throw msg + new Error().stack;
    }
Module['FS_createPath']('/', 'Assets', true, true);
Module['FS_createPath']('/Assets', 'models', true, true);
Module['FS_createPath']('/Assets/models', 'nanosuit', true, true);
Module['FS_createPath']('/Assets', 'skybox', true, true);
Module['FS_createPath']('/Assets/skybox', 'islands', true, true);
Module['FS_createPath']('/Assets/skybox', 'starfield', true, true);
Module['FS_createPath']('/Assets/skybox', 'space', true, true);
Module['FS_createPath']('/Assets', 'shaders', true, true);
Module['FS_createPath']('/Assets', 'fonts', true, true);

    function DataRequest(start, end, crunched, audio) {
      this.start = start;
      this.end = end;
      this.crunched = crunched;
      this.audio = audio;
    }
    DataRequest.prototype = {
      requests: {},
      open: function(mode, name) {
        this.name = name;
        this.requests[name] = this;
        Module['addRunDependency']('fp ' + this.name);
      },
      send: function() {},
      onload: function() {
        var byteArray = this.byteArray.subarray(this.start, this.end);

          this.finish(byteArray);

      },
      finish: function(byteArray) {
        var that = this;
        Module['FS_createPreloadedFile'](this.name, null, byteArray, true, true, function() {
          Module['removeRunDependency']('fp ' + that.name);
        }, function() {
          if (that.audio) {
            Module['removeRunDependency']('fp ' + that.name); // workaround for chromium bug 124926 (still no audio with this, but at least we don't hang)
          } else {
            Module.printErr('Preloading file ' + that.name + ' failed');
          }
        }, false, true); // canOwn this data in the filesystem, it is a slide into the heap that will never change
        this.requests[this.name] = null;
      },
    };

      new DataRequest(0, 5127, 0, 0).open('GET', '/Assets/test.png');
    new DataRequest(5127, 13851, 0, 0).open('GET', '/Assets/stone.jpg');
    new DataRequest(13851, 29626, 0, 0).open('GET', '/Assets/null.png');
    new DataRequest(29626, 1771241, 0, 0).open('GET', '/Assets/models/nanosuit/hand_showroom_spec.png');
    new DataRequest(1771241, 2637263, 0, 0).open('GET', '/Assets/models/nanosuit/arm_showroom_ddn.png');
    new DataRequest(2637263, 3384048, 0, 0).open('GET', '/Assets/models/nanosuit/helmet_showroom_ddn.png');
    new DataRequest(3384048, 4243905, 0, 0).open('GET', '/Assets/models/nanosuit/body_showroom_ddn.png');
    new DataRequest(4243905, 4249893, 0, 0).open('GET', '/Assets/models/nanosuit/glass_ddn.png');
    new DataRequest(4249893, 4825139, 0, 0).open('GET', '/Assets/models/nanosuit/arm_dif.png');
    new DataRequest(4825139, 5824790, 0, 0).open('GET', '/Assets/models/nanosuit/hand_showroom_ddn.png');
    new DataRequest(5824790, 8795418, 0, 0).open('GET', '/Assets/models/nanosuit/nanosuit.blend');
    new DataRequest(8795418, 9431693, 0, 0).open('GET', '/Assets/models/nanosuit/body_dif.png');
    new DataRequest(9431693, 9433089, 0, 0).open('GET', '/Assets/models/nanosuit/nanosuit.mtl');
    new DataRequest(9433089, 9434392, 0, 0).open('GET', '/Assets/models/nanosuit/glass_dif.png');
    new DataRequest(9434392, 10805130, 0, 0).open('GET', '/Assets/models/nanosuit/leg_showroom_ddn.png');
    new DataRequest(10805130, 12572339, 0, 0).open('GET', '/Assets/models/nanosuit/body_showroom_spec.png');
    new DataRequest(12572339, 13227556, 0, 0).open('GET', '/Assets/models/nanosuit/hand_dif.png');
    new DataRequest(13227556, 14999897, 0, 0).open('GET', '/Assets/models/nanosuit/leg_showroom_spec.png');
    new DataRequest(14999897, 15525161, 0, 0).open('GET', '/Assets/models/nanosuit/helmet_diff.png');
    new DataRequest(15525161, 17090477, 0, 0).open('GET', '/Assets/models/nanosuit/helmet_showroom_spec.png');
    new DataRequest(17090477, 18270222, 0, 0).open('GET', '/Assets/models/nanosuit/leg_dif.png');
    new DataRequest(18270222, 19937740, 0, 0).open('GET', '/Assets/models/nanosuit/arm_showroom_spec.png');
    new DataRequest(19937740, 20475778, 0, 0).open('GET', '/Assets/skybox/islands/right.jpg');
    new DataRequest(20475778, 21077663, 0, 0).open('GET', '/Assets/skybox/islands/left.jpg');
    new DataRequest(21077663, 21423802, 0, 0).open('GET', '/Assets/skybox/islands/top.jpg');
    new DataRequest(21423802, 22163870, 0, 0).open('GET', '/Assets/skybox/islands/front.jpg');
    new DataRequest(22163870, 22637199, 0, 0).open('GET', '/Assets/skybox/islands/back.jpg');
    new DataRequest(22637199, 22917788, 0, 0).open('GET', '/Assets/skybox/islands/bottom.jpg');
    new DataRequest(22917788, 23704238, 0, 0).open('GET', '/Assets/skybox/starfield/right.tga');
    new DataRequest(23704238, 24490688, 0, 0).open('GET', '/Assets/skybox/starfield/front.tga');
    new DataRequest(24490688, 25277138, 0, 0).open('GET', '/Assets/skybox/starfield/bottom.tga');
    new DataRequest(25277138, 26063588, 0, 0).open('GET', '/Assets/skybox/starfield/back.tga');
    new DataRequest(26063588, 26850038, 0, 0).open('GET', '/Assets/skybox/starfield/left.tga');
    new DataRequest(26850038, 27636488, 0, 0).open('GET', '/Assets/skybox/starfield/top.tga');
    new DataRequest(27636488, 28711557, 0, 0).open('GET', '/Assets/skybox/space/right.png');
    new DataRequest(28711557, 29682482, 0, 0).open('GET', '/Assets/skybox/space/left.png');
    new DataRequest(29682482, 30406849, 0, 0).open('GET', '/Assets/skybox/space/front.png');
    new DataRequest(30406849, 31116463, 0, 0).open('GET', '/Assets/skybox/space/back.png');
    new DataRequest(31116463, 32089974, 0, 0).open('GET', '/Assets/skybox/space/top.png');
    new DataRequest(32089974, 32968708, 0, 0).open('GET', '/Assets/skybox/space/bottom.png');
    new DataRequest(32968708, 32969857, 0, 0).open('GET', '/Assets/shaders/lighting.frag');
    new DataRequest(32969857, 32970198, 0, 0).open('GET', '/Assets/shaders/lighting.vert');
    new DataRequest(32970198, 32973738, 0, 0).open('GET', '/Assets/shaders/noise3D.glsl');
    new DataRequest(32973738, 32973873, 0, 0).open('GET', '/Assets/shaders/UI.vert');
    new DataRequest(32973873, 32974064, 0, 0).open('GET', '/Assets/shaders/Solar.vert');
    new DataRequest(32974064, 32974400, 0, 0).open('GET', '/Assets/shaders/RenderSystem2D.frag');
    new DataRequest(32974400, 32974552, 0, 0).open('GET', '/Assets/shaders/Model.frag');
    new DataRequest(32974552, 32974751, 0, 0).open('GET', '/Assets/shaders/Solar.frag');
    new DataRequest(32974751, 32974975, 0, 0).open('GET', '/Assets/shaders/Text.vert');
    new DataRequest(32974975, 32975684, 0, 0).open('GET', '/Assets/shaders/Jovian.frag');
    new DataRequest(32975684, 32976543, 0, 0).open('GET', '/Assets/shaders/Jovian.vert');
    new DataRequest(32976543, 32976677, 0, 0).open('GET', '/Assets/shaders/wireframe.vert');
    new DataRequest(32976677, 32976768, 0, 0).open('GET', '/Assets/shaders/UI.frag');
    new DataRequest(32976768, 32976883, 0, 0).open('GET', '/Assets/shaders/unlight.frag');
    new DataRequest(32976883, 32977031, 0, 0).open('GET', '/Assets/shaders/skybox.frag');
    new DataRequest(32977031, 32977329, 0, 0).open('GET', '/Assets/shaders/RenderSystem2D.vert');
    new DataRequest(32977329, 32977572, 0, 0).open('GET', '/Assets/shaders/Terrain.frag');
    new DataRequest(32977572, 32977786, 0, 0).open('GET', '/Assets/shaders/Text.frag');
    new DataRequest(32977786, 32978043, 0, 0).open('GET', '/Assets/shaders/skybox.vert');
    new DataRequest(32978043, 32978326, 0, 0).open('GET', '/Assets/shaders/Terrain.vert');
    new DataRequest(32978326, 32978530, 0, 0).open('GET', '/Assets/shaders/sprite.frag');
    new DataRequest(32978530, 32978774, 0, 0).open('GET', '/Assets/shaders/Model.vert');
    new DataRequest(32978774, 32978987, 0, 0).open('GET', '/Assets/shaders/sprite.vert');
    new DataRequest(32978987, 32979058, 0, 0).open('GET', '/Assets/shaders/wireframe.frag');
    new DataRequest(32979058, 33757610, 0, 0).open('GET', '/Assets/fonts/arial.ttf');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though
      // (we may be allocating before malloc is ready, during startup).
      var ptr = Module['getMemory'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/Assets/test.png"].onload();
          DataRequest.prototype.requests["/Assets/stone.jpg"].onload();
          DataRequest.prototype.requests["/Assets/null.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/hand_showroom_spec.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/arm_showroom_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/helmet_showroom_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/body_showroom_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/glass_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/arm_dif.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/hand_showroom_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/nanosuit.blend"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/body_dif.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/nanosuit.mtl"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/glass_dif.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/leg_showroom_ddn.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/body_showroom_spec.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/hand_dif.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/leg_showroom_spec.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/helmet_diff.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/helmet_showroom_spec.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/leg_dif.png"].onload();
          DataRequest.prototype.requests["/Assets/models/nanosuit/arm_showroom_spec.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/right.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/left.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/top.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/front.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/back.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/islands/bottom.jpg"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/right.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/front.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/bottom.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/back.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/left.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/starfield/top.tga"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/right.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/left.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/front.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/back.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/top.png"].onload();
          DataRequest.prototype.requests["/Assets/skybox/space/bottom.png"].onload();
          DataRequest.prototype.requests["/Assets/shaders/lighting.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/lighting.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/noise3D.glsl"].onload();
          DataRequest.prototype.requests["/Assets/shaders/UI.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Solar.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/RenderSystem2D.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Model.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Solar.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Text.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Jovian.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Jovian.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/wireframe.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/UI.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/unlight.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/skybox.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/RenderSystem2D.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Terrain.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Text.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/skybox.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Terrain.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/sprite.frag"].onload();
          DataRequest.prototype.requests["/Assets/shaders/Model.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/sprite.vert"].onload();
          DataRequest.prototype.requests["/Assets/shaders/wireframe.frag"].onload();
          DataRequest.prototype.requests["/Assets/fonts/arial.ttf"].onload();
          Module['removeRunDependency']('datafile_assets');

    };
    Module['addRunDependency']('datafile_assets');
  
    if (!Module.preloadResults) Module.preloadResults = {};
  
      Module.preloadResults[PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }
    
  }
  if (Module['calledRun']) {
    runWithFS();
  } else {
    if (!Module['preRun']) Module['preRun'] = [];
    Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
  }

 }
 loadPackage();

})();
