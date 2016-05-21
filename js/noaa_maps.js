//-------------------------------GPL-------------------------------------//
//
// MetOcean Viewer - A simple interface for viewing hydrodynamic model data
// Copyright (C) 2015  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------//
var map;
var CurrentID = -1;
var CurrentName;
var CurrentLat;
var CurrentLon;

function returnStationID(){
    var answer = String(CurrentID)+";"+CurrentName+";"+String(CurrentLon)+";"+String(CurrentLat);
    return answer;
}

////////////////////////////////////////////////////////////
//   PAN TO FUNCTION                                      //
//                                                        //
//  Function to pan to a different area on the map        //
////////////////////////////////////////////////////////////
function panTo(location){
var geocoder = new google.maps.Geocoder();
    geocoder.geocode( { 'address': location}, function(results, status) {
        if (status === google.maps.GeocoderStatus.OK) {
          map.setCenter(results[0].geometry.location);
          map.fitBounds(results[0].geometry.viewport);
        }
    });
}

////////////////////////////////////////////////////////////
//   INITIALIZE FUNCTION                                  //
//                                                        //
//  Function to initialize the google maps area on load   //
////////////////////////////////////////////////////////////
function initialize() {
//Initialize the map

    window.LastInfo = -1;
    var myOptions = {
      center: new google.maps.LatLng(29.5, -91.5),
      zoom: 4,
      mapTypeId: google.maps.MapTypeId.ROADMAP,
      panControl: true,
      streetViewControl: false,
      panControlOptions: { position: google.maps.ControlPosition.LEFT_TOP },
      zoomControlOptions: { position: google.maps.ControlPosition.LEFT_TOP }
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);

    var layer = new google.maps.FusionTablesLayer({
        query: {
            select: '\'Geocodable address\'',
            from: '15d2USe6y0Atqt09PUJpzhBC2HmBs3Ap6IEMt-uEs'
        },
        options: {
            styleId: 2,
            templateId: 2
        }
    });
    layer.setMap(map);

    google.maps.event.addListener(layer, 'click', function(event) {
        CurrentID = event.row.StationID.value;
        CurrentName = event.row.StationName.value;
        CurrentLat = event.row.Latitude.value;
        CurrentLon = event.row.Longitude.value;
    });

    var geocoder = new google.maps.Geocoder();
    geocoder.geocode( { 'address': "United States"}, function(results, status) {
        if (status == google.maps.GeocoderStatus.OK) {
            map.setCenter(results[0].geometry.location);
            map.fitBounds(results[0].geometry.viewport);
        }
    });

}

google.maps.event.addDomListener(window, "resize", function() {
    var center = map.getCenter();
    google.maps.event.trigger(map, "resize");
    map.setCenter(center);
});



