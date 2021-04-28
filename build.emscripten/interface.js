const boardWidth = 960;
const boardHeight = boardWidth;
const spaceToBoardRatio = 0.125;
const sideCount = 4;
const spacesPerSide = 10;
const spaceHeight = spaceToBoardRatio * boardHeight;
const spaceWidth = (boardWidth - (2 * spaceHeight)) / (spacesPerSide - 1);
const pxPosOffset = spaceWidth - spaceHeight;
const iconToSpaceRatio = 0.8;
const pieceToSpaceRatio = 0.125;
const priceToSpaceRatio = 0.2;
const colorBannerToSpaceRatio = 0.2;
const spaceIconSize = iconToSpaceRatio * spaceWidth;
const pieceIconSize = pieceToSpaceRatio * spaceHeight;
const priceFontSize = priceToSpaceRatio * spaceWidth;
const colorBannerHeight = colorBannerToSpaceRatio * spaceHeight;
const highlightColor = '#4891ff';
const borderColor = 'Black';
const boardColor = '#CDE6D0'; // Monopoly board teal
const basicIconColor = '#726E6D'; // Smokey Gray
const bidIncrements = [
    5,
    10,
    25,
    50,
];

var manageModeOn = false;
var tradePartner = -1;
var selectedProperty = -1;
var trade = null;

var getPlayerIndex = function (gameState) {
    return gameState.get_controlling_player_index();
};
var addToBid = function (state, increment) {
    return state.get_current_auction ().highestBid + increment;
}

let resetTrade = (offeringPlayerIndex, consideringPlayerIndex) => {
    trade = {
        offeringPlayer: offeringPlayerIndex,
        consideringPlayer: consideringPlayerIndex,
        offer: {
            cash: 0,
            deeds: new Set (),
            getOutOfJailFreeCards: [],
        },
        consideration: {
            cash: 0,
            deeds: new Set (),
            getOutOfJailFreeCards: [],
        }
    };
}

var setElementVisibility = (element, visible) => {
    if (visible) {
        element.style.display = 'inline';
    }
    else {
        element.style.display = 'none';
    }
};

function updateInputButtons(gameState) {
    const thisPlayerIndex = getPlayerIndex(gameState);
    const tradeModeOn = tradePartner != -1;
    const altModeOn = manageModeOn || tradePartner != -1;
    setElementVisibility(rollButton, ! altModeOn && gameState.check_if_player_is_allowed_to_roll(thisPlayerIndex));
    setElementVisibility(buyPropertyButton, ! altModeOn && gameState.check_if_player_is_allowed_to_buy_property(thisPlayerIndex));
    setElementVisibility(auctionPropertyButton, ! altModeOn && gameState.check_if_player_is_allowed_to_auction_property(thisPlayerIndex));
    setElementVisibility(useGetOutOfJailFreeCardButton, ! altModeOn && gameState.check_if_player_is_allowed_to_use_get_out_jail_free_card(thisPlayerIndex));
    setElementVisibility(payBailButton, ! altModeOn && gameState.check_if_player_is_allowed_to_pay_bail(thisPlayerIndex));
    for (i = 0; i < bidIncrements.length; ++i) {
        const increment = bidIncrements[i];
        setElementVisibility(bidButton(increment), ! altModeOn && gameState.check_if_player_is_allowed_to_bid(thisPlayerIndex, addToBid (gameState, increment)));
    }
    setElementVisibility(auctionAmountLabel, ! altModeOn && gameState.get_turn_phase() == Module.TurnPhase.WaitingForBids);
    setElementVisibility(declineBidButton, ! altModeOn && gameState.check_if_player_is_allowed_to_decline_bid(thisPlayerIndex));
    setElementVisibility(endTurnButton, ! altModeOn && gameState.check_if_player_is_allowed_to_end_turn(thisPlayerIndex));
    setElementVisibility(resignButton, gameState.check_if_player_is_allowed_to_resign(thisPlayerIndex));
    setElementVisibility(startManageButton, ! altModeOn && gameState.get_controlling_player_index() == thisPlayerIndex);
    setElementVisibility(finishManageButton, manageModeOn && gameState.get_controlling_player_index() == thisPlayerIndex);
    setElementVisibility(mortgageButton, manageModeOn && gameState.check_if_player_is_allowed_to_mortgage(thisPlayerIndex, selectedProperty));
    setElementVisibility(unmortgageButton, manageModeOn && gameState.check_if_player_is_allowed_to_unmortgage(thisPlayerIndex, selectedProperty));
    setElementVisibility(sellBuildingButton, manageModeOn && gameState.check_if_player_is_allowed_to_sell_building(thisPlayerIndex, selectedProperty));
    setElementVisibility(buyBuildingButton, manageModeOn && gameState.check_if_player_is_allowed_to_buy_building(thisPlayerIndex, selectedProperty));
    setElementVisibility(declineTradeButton, tradeModeOn && gameState.check_if_player_is_allowed_to_decline_trade(thisPlayerIndex));
    setElementVisibility(cancelTradeButton, tradeModeOn && ! gameState.check_if_player_is_allowed_to_decline_trade(thisPlayerIndex));
    setElementVisibility(diceLabel, ! tradeModeOn);
    setElementVisibility(tradeTable, tradeModeOn);

    const playerSpace = gameState.get_player_position(gameState.get_active_player_index());
    if (Module.space_is_property(playerSpace)) {
        const propertyPrice = Module.price_of_property(Module.space_to_property(playerSpace));
        var propertyPriceLabel = document.getElementById("propertyPriceLabel");
        propertyPriceLabel.innerHTML = "$" + propertyPrice; 
    }
}

let createIconHTML = (iconName) => {
    iconElement = document.createElement("i");
    iconElement.setAttribute("class", "small-icon fas fa-" + iconName);
    return iconElement;
}

let addPlayerTableRow = (gameState, table, playerCount, playerIndex) => {
    let row = table.insertRow();
    row.setAttribute("id", "playerTableRow_" + playerIndex)
    row.setAttribute("display", "block");
    let nameCell = row.insertCell();
    nameCell.appendChild (document.createTextNode(Module.player_name(playerIndex)))
    let fundsCell = row.insertCell();
    row.setAttribute("height", 100 / playerCount + "%");
    fundsCell.setAttribute("id", "playerTableFunds_" + playerIndex)
    fundsCell.setAttribute("width", "40%");
    fundsCell.appendChild (document.createTextNode("1500"))
    let tradeButtonCell = row.insertCell();
    let tradeButton = document.createElement("button");
    tradeButtonCell.appendChild(tradeButton);
    tradeButton.setAttribute("id", "tradeWithPlayerButton_" + playerIndex);
    tradeButton.setAttribute("type", "button");
    tradeButton.setAttribute("class", "button");
    tradeButton.appendChild(createIconHTML("exchange-alt"));
    tradeButton.onclick = function () {
        tradePartner = playerIndex;
        manageModeOn = false;
        updateInputButtons(gameState);
        resetTrade(gameState.get_active_player_index(), tradePartner);
        rebuildTradeTable();
    };
}

let updatePlayerTable = (gameState) => {
    let table = document.getElementById("playerTable");
    const controllingPlayerIndex = gameState.get_controlling_player_index();
    
    for (let p = 0; p < table.rows.length; ++p) {
        let row = table.rows[p];
        if (gameState.get_player_eliminated(p)) {
            row.style.color = 'silver';
            row.style.background = 'white';
        }
        else if (controllingPlayerIndex == p) {
            row.style.backgroundColor = playerColors[p];
            row.style.color = 'white';
        }
        else {
            row.style.background = 'white';
            row.style.color = playerColors[p];
        }
        let fundsCell = document.getElementById("playerTableFunds_" + p);
        let tradeButton = document.getElementById("tradeWithPlayerButton_" + p);
        fundsCell.textContent = gameState.get_player_funds(p);
        if (gameState.check_if_player_is_allowed_to_trade_with_player (controllingPlayerIndex, p)) {
            tradeButton.style.visibility = 'visible';
        }
        else {
            tradeButton.style.visibility = 'hidden';
        }
    }
}

let buildPlayerTable = (gameState) => {
    let table = document.getElementById("playerTable");
    const playerCount = gameState.get_player_count();
    
    for (let p = 0; p < playerCount; ++p) {
        addPlayerTableRow(gameState, table, playerCount, p);
    }
}

let groupIsColored = (group) => {
    return group != Module.PropertyGroup.Railroad && group != Module.PropertyGroup.Utility;
}

let groupToColor = (group) => {
    switch (group) {
        case Module.PropertyGroup.Brown:
            return "#590C38";
        case Module.PropertyGroup.LightBlue:
            return "#87A5D7";
        case Module.PropertyGroup.Magenta:
            return "#EF3878";
        case Module.PropertyGroup.Orange:
            return "#F67F23";
        case Module.PropertyGroup.Red:
            return "#EF3A25";
        case Module.PropertyGroup.Yellow:
            return "#FEE703";
        case Module.PropertyGroup.Green:
            return "#13A55C";
        case Module.PropertyGroup.Blue:
            return "#284EA1";
        case Module.PropertyGroup.Railroad:
            return "#000000";
        case Module.PropertyGroup.Utility:
            return "#483C32";
    }
}

// Stack Overflow
// "How to decide font color in white or black depending on background color?"
// https://stackoverflow.com/a/41491220
// - SudoPlz
function pickTextColorBasedOnBgColor(bgColor, lightColor = '#FFFFFF', darkColor = '#000000') {
    var color = (bgColor.charAt(0) === '#') ? bgColor.substring(1, 7) : bgColor;
    var r = parseInt(color.substring(0, 2), 16); // hexToR
    var g = parseInt(color.substring(2, 4), 16); // hexToG
    var b = parseInt(color.substring(4, 6), 16); // hexToB
    var uicolors = [r / 255, g / 255, b / 255];
    var c = uicolors.map((col) => {
        if (col <= 0.03928) {
            return col / 12.92;
        }
        return Math.pow((col + 0.055) / 1.055, 2.4);
    });
    var L = (0.2126 * c[0]) + (0.7152 * c[1]) + (0.0722 * c[2]);
    return (L > 0.179) ? darkColor : lightColor;
}

let addCell = (tableRow, rowIndex, items) => {
    let cell = tableRow.insertCell();
    let property = items[rowIndex];
    cell.setAttribute("width", "50%");
    if (rowIndex < items.length) {
        let bgColor = groupToColor(Module.property_group(property));
        cell.style.backgroundColor = bgColor;
        cell.style.color = pickTextColorBasedOnBgColor(bgColor);
        cell.appendChild(document.createTextNode(Module.property_to_string(property)));
    }
}

let rebuildTradeTable = () => {
    tradeTable.innerHTML = "";
    let thead = tradeTable.createTHead();
    let headerRow = thead.insertRow();
    let offerHeader = document.createElement("th");
    offerHeader.appendChild(document.createTextNode ("Offer"));
    let considerationHeader = document.createElement("th");
    considerationHeader.appendChild(document.createTextNode ("Consideration"));
    headerRow.appendChild(offerHeader);
    headerRow.appendChild(considerationHeader);

    let offers = [... trade.offer.deeds];
    let considerations = [... trade.consideration.deeds];
    const offerDeedCount = offers.length;
    const considerationDeedCount = considerations.length;
    const rowCount = Math.max(offerDeedCount, considerationDeedCount);
    
    for (let r = 0; r < rowCount; ++r) {
        let row = tradeTable.insertRow();
        row.setAttribute("height", "20%");
        row.setAttribute("width", "100%");
        row.setAttribute("display", "block");
        addCell(row, r, offers);
        addCell(row, r, considerations);
    }
}

const playerIcons = [
    "\uf21a", // fa-ship
    "\uf544", // fa-robot
    "\uf0f4", // fa-coffee
    "\uf5fc", // fa-laptop-code
];

const playerColors = [
    '#4863A0', // Steel Blue
    '#4E8975', // Sea Green
    '#6F4E37', // Coffee
    '#E18B6B', // Caramel
];

var bidButton = (bidIncrement) => {
    return document.getElementById("bidButton_" + bidIncrement);
}

var dieIconName = (faceValue) => {
    switch (faceValue) {
        case 1: return "one";
        case 2: return "two";
        case 3: return "three";
        case 4: return "four";
        case 5: return "five";
        case 6: return "six";
    }
    return "d6";
}

var gameFont = (pixelSize) => {
    return makeFont(pixelSize, "Righteous");
};
var iconFont = (pixelSize) => {
    return makeFont(pixelSize, "FontAwesome");
};
var makeFont = (pixelSize, fontFamily) => {
    return pixelSize + "px " + fontFamily;
};

var Module = {
    onRuntimeInitialized: function () {
        var rollButton = document.getElementById("rollButton");
        var buyPropertyButton = document.getElementById("buyPropertyButton");
        var auctionPropertyButton = document.getElementById("auctionPropertyButton");
        var useGetOutOfJailFreeCardButton = document.getElementById("useGetOutOfJailFreeCardButton");
        var payBailButton = document.getElementById("payBailButton");
        var declineBidButton = document.getElementById("declineBidButton");
        var endTurnButton = document.getElementById("endTurnButton");
        var resignButton = document.getElementById("resignButton");
        var startManageButton = document.getElementById("startManageButton");
        var finishManageButton = document.getElementById("finishManageButton");
        var mortgageButton = document.getElementById("mortgageButton");
        var unmortgageButton = document.getElementById("unmortgageButton");
        var sellBuildingButton = document.getElementById("sellBuildingButton");
        var buyBuildingButton = document.getElementById("buyBuildingButton");
        //var offerTradeButton = document.getElementById("offerTradeButton");
        var declineTradeButton = document.getElementById("declineTradeButton");
        var cancelTradeButton = document.getElementById("cancelTradeButton");
        var tradeTable = document.getElementById("tradeTable");

        var gameState = new Module.GameState;
        var JavascriptInterface = Module.SimpleInterface.extend("SimpleInterface", {
            waitForInput: function () {
                updateInputButtons(gameState);
                paintBoard(gameState);
            },
            // C++ override
            get_setup: function () {
                return {
                    seed: "monopolyRandomSeed!",
                    playerCount: 2,
                    startingFunds: 1500,
                };
            },
            // C++ override
            update: function (state_) {
                gameState.assign (state_);
                this.waitForInput();
            },
        });

        // Ensure font awesome is loaded before we draw icons
        setTimeout(start, 100) // Yucky, we should actually wait until font awesome is loaded
        function start() {
            var interface = new JavascriptInterface;
            var game = new Module.Game(interface);
            buildPlayerTable(gameState);

            let getSpaceClicked = (canvas, event) => {
                const boundingRect = canvas.getBoundingClientRect();
                const x = event.clientX - boundingRect.left;
                const y = event.clientY - boundingRect.top;
                const top = y <= spaceHeight;
                const bot = y >= boardHeight - spaceHeight;
                const left = x <= spaceHeight;
                const right = x >= boardWidth - spaceHeight;

                let side = 0;
                let pxPos = 0;
                if (bot && !left) {
                    side = 0;
                    pxPos = (boardWidth - x) + pxPosOffset;
                }
                if (left && !top) {
                    side = 1;
                    pxPos = (boardHeight - y) + pxPosOffset
                }
                else if (top && !right) {
                    side = 2;
                    pxPos = x + pxPosOffset;
                }
                else if (right && !bot) {
                    side = 3;
                    pxPos = y + pxPosOffset;
                }

                if (pxPos < 0) {
                    pxPos = 0;
                }
                const indexAlongSide = Math.floor(pxPos / spaceWidth);
                const spaceIndex = side * 10 + indexAlongSide;
                const space = Module.index_to_space(spaceIndex);
                return space;
            }

            let selectProperty = (gameState, property) => {
                selectedProperty = property;
                updateInputButtons(gameState);
                paintBoard(gameState);
            }

            let selectForDeeds = (deeds, property) => {
                if (deeds.has (property))
                    deeds.delete(property);
                else
                    deeds.add(property);
                rebuildTradeTable();
            }
            let selectForOffer = (property) => {
                selectForDeeds(trade.offer.deeds, property);
            }
            let selectForConsideration = (property) => {
                selectForDeeds(trade.consideration.deeds, property);
            }

            var canvas = document.getElementById("boardCanvas");

            canvas.addEventListener("click", function (e) {
                const canSelectProperties = manageModeOn || tradePartner != -1;
                if (canSelectProperties) {
                    const space = getSpaceClicked(canvas, e);
                    const property = Module.space_to_property(space);
                    if (property != Module.Property.Invalid) {
                        const ownerIndex = gameState.get_property_owner_index(property);
                        if (manageModeOn) {
                            if (ownerIndex == getPlayerIndex(gameState)) {
                                if (selectedProperty != property) {
                                    selectProperty(gameState, property)
                                    return;
                                }
                            }
                        }
                        else if (tradePartner != -1) {
                            if (ownerIndex == getPlayerIndex(gameState)) {
                                selectForOffer(property);
                                return;
                            }
                            else if (ownerIndex == tradePartner) {
                                selectForConsideration(property);
                                return;
                            }
                        }
                    }
                }
                selectProperty(gameState, Module.Property.Invalid);
            }, false);

            rollButton.onclick = function () {
                interface.roll_dice(getPlayerIndex (gameState));
                game.process();
            };
            buyPropertyButton.onclick = function () {
                interface.buy_property(getPlayerIndex (gameState));
                game.process();
            };
            auctionPropertyButton.onclick = function () {
                interface.auction_property(getPlayerIndex (gameState));
                game.process();
            };
            useGetOutOfJailFreeCardButton.onclick = function () {
                interface.use_get_out_of_jail_free_card(getPlayerIndex (gameState));
                game.process();
            };
            payBailButton.onclick = function () {
                interface.pay_bail(getPlayerIndex (gameState));
                game.process();
            };
            for (i = 0; i < bidIncrements.length; ++i) {
                const increment = bidIncrements[i];
                bidButton(increment).onclick = function () {
                    interface.bid(getPlayerIndex (gameState), addToBid(gameState, increment));
                    game.process();
                };
            }
            declineBidButton.onclick = function () {
                interface.decline_bid(getPlayerIndex (gameState));
                game.process();
            };
            endTurnButton.onclick = function () {
                interface.end_turn(getPlayerIndex (gameState));
                game.process();
            };
            resignButton.onclick = function () {
                interface.resign(getPlayerIndex (gameState));
                game.process();
            };
            startManageButton.onclick = function () {
                manageModeOn = true;
                updateInputButtons(gameState);
                paintBoard(gameState);
            };
            finishManageButton.onclick = function () {
                manageModeOn = false;
                selectedProperty = -1;
                updateInputButtons(gameState);
                paintBoard(gameState);
            };
            mortgageButton.onclick = function () {
                interface.mortgage_property(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
            unmortgageButton.onclick = function () {
                interface.unmortgage_property(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
            sellBuildingButton.onclick = function () {
                interface.sell_building(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
            buyBuildingButton.onclick = function () {
                interface.buy_building(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
            declineTradeButton.onclick = function () {
                interface.declineTradeButton(getPlayerIndex (gameState));
                game.process();
            };
            cancelTradeButton.onclick = function () {
                tradePartner = -1;
                updateInputButtons(gameState);
                paintBoard(gameState);
            };
        }
        function paintBoard(gameState) {

            let translateToSpace = (ctx, space) => {
                const index = Module.space_to_index(space);
                const side = Math.floor (index / 10);
                const pos = index % 10;

                // Rotate about center
                ctx.translate(boardWidth / 2, boardHeight / 2);
                ctx.rotate(side * Math.PI / 2);
                ctx.translate(-boardWidth / 2, -boardHeight / 2);

                // Go to top left corner of space on this side (should be facing the up at this rotation)
                ctx.translate(boardWidth - (spaceHeight + spaceWidth * pos), boardHeight - spaceHeight);
            }

            let drawPlayerPiece = (ctx, playerIndex, space) => {
                const icon = playerIcons[playerIndex];
                const color = playerColors[playerIndex];

                ctx.save();
                translateToSpace(ctx, space);
                ctx.translate(spaceWidth / 2, spaceHeight / 2);
                const x = (playerIndex % 2 == 0 ? -1 : 1) * spaceWidth / 4;
                const y = (playerIndex < 2 == 0 ? -1 : 1) * spaceHeight / 6;
                ctx.translate(x, y);
                ctx.beginPath();
                ctx.arc(0, 0, pieceIconSize / 2 + spaceWidth/10, 0, 2 * Math.PI, false);
                ctx.fillStyle = color;
                ctx.strokeStyle = borderColor;
                ctx.fill();
                ctx.stroke();
                drawIcon(ctx, 'White', pieceIconSize,  icon);
                ctx.restore();
            }

            let drawPlayerPieces = (ctx) => {
                for (let p = 0; p < gameState.get_player_count(); ++p) {
                    const space = gameState.get_player_position(p);
                    drawPlayerPiece(ctx, p, space);
                }
            };

            let updateDiceDisplay = () => {
                var d1 = document.getElementById("die1");
                var d2 = document.getElementById("die2");
                var diceValues = gameState.get_last_dice_roll();
                const activePlayerIndex = gameState.get_active_player_index();
                const dieColor = playerColors[activePlayerIndex];

                let updateDieDisplay = (dieElement, value) => {
                    if (Module.can) {
                        dieElement.setAttribute("class", "dice fas fa-dice-d6");
                    }
                    else {
                        dieElement.setAttribute("class", "dice fas fa-dice-" + dieIconName (value));
                    }
                    dieElement.style.color = dieColor;
                }
                updateDieDisplay(d1, diceValues.first);
                updateDieDisplay(d2, diceValues.second);
            }

            let updateButtonColors = () => {
                const controllingPlayerIndex = gameState.get_controlling_player_index();
                const buttonColor = playerColors[controllingPlayerIndex];
                var buttons = document.getElementsByClassName("button");
                for (i = 0; i < buttons.length; ++i) { 
                    buttons[i].style.backgroundColor = buttonColor;
                }
            }

            let updateAuctionLabels = () => {
                var amountLabel = document.getElementById("auctionAmountLabel");
                const auction = gameState.get_current_auction();
                const auctionIsActive = auction.biddingOrder.size() > 0;
                setElementVisibility(amountLabel, auctionIsActive);
                if (auctionIsActive) {
                    const winningPlayerIndex = auction.biddingOrder.get(auction.biddingOrder.size () - 1);
                    const labelColor = playerColors[winningPlayerIndex];
                    amountLabel.innerHTML = "$" + auction.highestBid;
                    amountLabel.style.color = labelColor;
                }
            }

            let isCornerSpace = (space) => {
                return space == Module.Space.Go
                    || space == Module.Space.Jail
                    || space == Module.Space.FreeParking
                    || space == Module.Space.GoToJail;
            }

            let drawPropertySpace = (ctx, space) => {
                const property = Module.space_to_property(space);
                const group = Module.property_group(property);
                const color = groupToColor(group);
                if (groupIsColored (group)) {
                    drawColorBar(ctx, color);
                    drawBuildings(ctx, gameState.get_building_level(property));
                }
                else {
                    drawNormalSpace(ctx, space, color);
                }
                drawPropertyPrice(ctx, property);
                const playerIndex = gameState.get_property_owner_index(property);
                if (playerIndex != -1) {
                    if (!manageModeOn) {
                        drawPropertyOwnershipIndicator(ctx, playerIndex);
                    }
                    else if (property == selectedProperty){
                        drawSelectionIndicator(ctx);
                    }
                }
                if (gameState.get_property_is_mortgaged(property)) {
                    drawMortgagedIndicator(ctx);
                }
            }

            let drawColorBar = (ctx, color) => {
                ctx.save();
                ctx.fillStyle = color;
                ctx.strokeStyle = borderColor;
                ctx.beginPath();
                ctx.rect(0, 0, spaceWidth, colorBannerHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
            }

            let drawBuilding = (ctx) => {
                const houseWidth = spaceWidth * 0.2; 
                const houseHeight = colorBannerHeight * 0.8;
                const housePoly = [
                    0, houseHeight * 0.25,
                    houseWidth * 0.5, 0,
                    houseWidth, houseHeight * 0.25,
                    houseWidth, houseHeight,
                    0, houseHeight];
                ctx.save();
                ctx.beginPath();
                ctx.moveTo(housePoly[0], housePoly[1]);
                for (let i = 2; i < housePoly.length - 1; i += 2) {
                    ctx.lineTo(housePoly[i], housePoly[i + 1]);
                }
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
            }

            let drawBuildings = (ctx, buildingLevel) => {
                const houseWidth = spaceWidth * 0.2; 
                ctx.save();
                if (buildingLevel <= 4) {
                    for (let i = 0; i < buildingLevel; ++i) {
                        ctx.save();
                        ctx.translate(spaceWidth * 0.025 + i * spaceWidth * 0.25, colorBannerHeight * 0.1);
                        ctx.fillStyle = 'Green';
                        ctx.strokeStyle = borderColor;
                        drawBuilding(ctx);
                        ctx.restore();
                    }
                }
                if (buildingLevel == 5) {
                    ctx.translate(spaceWidth * 0.5 - houseWidth * 0.5, colorBannerHeight * 0.1);
                    ctx.fillStyle = 'Red';
                    ctx.strokeStyle = borderColor;
                    drawBuilding(ctx);
                }
                ctx.restore();
            }

            let drawPropertyPrice = (ctx, property) => {
                ctx.fillStyle = borderColor;
                ctx.textAlign = 'center';
                ctx.textBaseline = 'bottom';
                ctx.font = gameFont(priceFontSize);
                ctx.fillText("$" + Module.price_of_property(property), spaceWidth / 2, spaceHeight - priceFontSize / 2);
            }

            let spaceToIcon = (space) => {
                switch (space) {
                    case Module.Space.Go:
                        return "\uf30a";
                    case Module.Space.Jail:
                        return "\uf6d9";
                    case Module.Space.FreeParking:
                        return "\uf540";
                    case Module.Space.GoToJail:
                        return "\uf0a6";
                    case Module.Space.CommunityChest_1:
                    case Module.Space.CommunityChest_2:
                    case Module.Space.CommunityChest_3:
                        return "\uf51e";
                    case Module.Space.Chance_1:
                    case Module.Space.Chance_2:
                    case Module.Space.Chance_3:
                        return "\uf128";
                    case Module.Space.IncomeTax:
                        return "\uf4c0";
                    case Module.Space.LuxuryTax:
                        return "\uf3a5";
                    case Module.Space.Railroad_1:
                    case Module.Space.Railroad_2:
                    case Module.Space.Railroad_3:
                    case Module.Space.Railroad_4:
                        return "\uf238";
                    case Module.Space.Utility_1:
                        return "\uf0eb";
                    case Module.Space.Utility_2:
                        return "\ue005";
                    default:
                        return "";
                }
            }

            let drawIcon = (ctx, color, pixelSize, icon) => {
                ctx.fillStyle = color;
                ctx.textAlign = 'center';
                ctx.textBaseline = 'middle';
                var font = iconFont(pixelSize);
                ctx.font = font;
                ctx.fillText(icon, 0, 0);
            }

            let operateOnSpaces = (ctx, drawFunction) => {
                for (let s = 0; s < sideCount; ++s) {
                    ctx.translate(0, boardHeight - spaceHeight);
                    for (let i = 0; i < spacesPerSide; ++i) {
                        const spaceIndex = s * 10 + i;
                        const isCornerSpace = spaceIndex % 10 == 0;
                        const space = Module.index_to_space(spaceIndex);
                        ctx.save();
                        ctx.translate(boardWidth - (spaceHeight + spaceWidth * i), 0);
                        drawFunction(ctx, space);
                        ctx.restore();
                    }
                    ctx.translate(0, spaceHeight - boardHeight);
                    ctx.translate(boardWidth / 2, boardHeight / 2);
                    ctx.rotate(Math.PI / 2);
                    ctx.translate(-boardWidth / 2, -boardHeight / 2);
                }
            }

            let drawNormalSpace = (ctx, space, color = basicIconColor) => {
                ctx.save();
                ctx.translate(spaceWidth / 2, spaceHeight / 2);
                drawIcon(ctx, color, spaceIconSize, spaceToIcon(space));
                ctx.restore();
            }

            let drawCornerSpace = (ctx, space) => {
                ctx.save();
                ctx.translate(spaceHeight / 2, spaceHeight / 2);
                ctx.rotate(-Math.PI / 4);
                drawIcon(ctx, basicIconColor, spaceIconSize, spaceToIcon(space));
                ctx.restore();
            }

            let drawSpace = (ctx, space) => {
                ctx.save();
                if (space == gameState.get_player_position(gameState.get_active_player_index()) &&
                    ! manageModeOn) {
                    ctx.fillStyle = highlightColor;
                }
                else {
                    ctx.fillStyle = boardColor;
                }
                ctx.beginPath();
                if (isCornerSpace(space)) {
                    ctx.rect(0, 0, spaceHeight, spaceHeight);
                }
                else {
                    ctx.rect(0, 0, spaceWidth, spaceHeight);
                }

                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
                if (isCornerSpace(space)) {
                    drawCornerSpace(ctx, space);
                }
                else {
                    if (Module.space_is_property(space)) {
                        drawPropertySpace(ctx, space);
                    }
                    else {
                        drawNormalSpace(ctx, space);
                    }
                }
            }

            let buildPathWithOwnedSpaces = (ctx, space) => {
                if (! Module.space_is_property(space)) {
                    return;
                }
                const property = Module.space_to_property(space);
                const owner = gameState.get_property_owner_index(property);
                if (owner == getPlayerIndex(gameState)) {
                    ctx.rect(0,0, spaceWidth, spaceHeight);
                }
            }

            let drawPropertyOwnershipIndicator = (ctx, player) => {
                ctx.save();
                ctx.beginPath();
                ctx.fillStyle = playerColors[player];
                ctx.strokeStyle = borderColor;
                ctx.rect(0, spaceHeight - colorBannerHeight, spaceWidth, spaceHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
            }

            let drawSelectionIndicator = (ctx) => {
                ctx.save();
                ctx.globalAlpha = 0.5;
                ctx.fillStyle = highlightColor;
                ctx.fillRect(0, 0, spaceWidth, spaceHeight);
                ctx.restore();
            }

            let drawMortgagedIndicator = (ctx) => {
                const x = spaceWidth * 0.125;
                const y = spaceHeight / 2;
                const signWidth = spaceWidth * 0.75;
                const signHeight = spaceWidth * 0.25;
                ctx.save();
                ctx.fillStyle = 'Red';
                ctx.strokeStyle = borderColor;
                ctx.beginPath();
                ctx.rect(x, y, signWidth, signHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.textAlign = 'center';
                ctx.textBaseline = 'middle';
                ctx.fillStyle = 'White';
                ctx.translate(x - signWidth / 2, y - signHeight / 2);
                ctx.font = gameFont(signHeight * 0.6);
                ctx.fillText("CLOSED", signWidth, signHeight);
                ctx.restore();
            }

            var canvas = document.getElementById("boardCanvas");

            var ctx = canvas.getContext("2d");
            ctx.globalCompositeOperation = "source-over";

            ctx.save();
            ctx.clearRect(0, 0, boardWidth, boardHeight);
            ctx.strokeRect(0, 0, boardWidth, boardHeight);
            operateOnSpaces(ctx, drawSpace)
            drawPlayerPieces(ctx);
            updatePlayerTable(gameState);
            updateDiceDisplay();
            updateButtonColors();
            updateAuctionLabels();
            ctx.restore();

            if (gameState.is_game_over()) {
                var buttons = document.getElementsByClassName("button");
                var diceLabel = document.getElementById("diceLabel");
                var gameOverLabel = document.getElementById("gameOverLabel");
                for (i = 0; i < buttons.length; ++i) { 
                    setElementVisibility(buttons[i], false);
                }
                setElementVisibility(diceLabel, false);
                setElementVisibility(gameOverLabel, true);
                if (gameState.get_player_eliminated(getPlayerIndex(gameState))) {
                    gameOverLabel.innerHTML = "You lose!";
                }
                else {
                    gameOverLabel.innerHTML = "You win!";
                }
            }
            else if (manageModeOn) {
                ctx.save();
                ctx.globalCompositeOperation = "saturation";
                ctx.beginPath();
                ctx.rect(0, 0, boardWidth, boardHeight);
                operateOnSpaces(ctx, buildPathWithOwnedSpaces)
                ctx.closePath();
                ctx.clip("evenodd");
                ctx.fillStyle = 'White';
                ctx.fillRect(0, 0, boardWidth, boardHeight);
                ctx.restore();
            }
        }
    }
}
