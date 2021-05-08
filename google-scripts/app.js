function testDate() {
  Logger.log(Utilities.formatDate(new Date(), "GMT+3", "MMM d, YYYY") + ' at ' + Utilities.formatDate(new Date(), "GMT+3", "hh:mm:ss"));
}

function doGet(e) { 
  Logger.log( JSON.stringify(e) );  // view parameters
  var result = 'Ok'; // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1LYgk3DTioQEZ8bXRQ_FzgyuSjljQnK51Pbf__i4cEMY';    // Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();   // get Active sheet
    var newRow = sheet.getLastRow() + 1;            
    var rowData = [];
    rowData[0] = Utilities.formatDate(new Date(), "GMT+3", "MMM d, YYYY") + ' at ' + Utilities.formatDate(new Date(), "GMT+3", "hh:mm:ss"); //timestamp
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'temperature':
          rowData[1] = value;
          result += ', Written temperature';
          break;
        case 'humidity':
          rowData[2] = value;
          result += ', Written humidity';
          break;
        case 'battery':
          rowData[3] = value;
          result += ', Written battery';
          break;  
        default:
          result = +"unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}