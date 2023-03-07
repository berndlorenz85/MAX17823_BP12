#ifndef __MAX17852_BALANCING_H_
#define __MAX17852_BALANCING_H_

/**
    * @brief   Gets Status Information of Balancing
    * @details Starts Reading Process of BALSTAT & BALUVSTAT for later Status Evaluation
    * @param BalCellData pointer to most recent cell measurement data
    * @param BalDCSize Size of Daisy Chain
    * @returns 0 if no errors -1 if error
    */
//char MAX17852_GetBalancingStatus(DeviceData *BalCellData, char BalDCSize);


/**
    * @brief   Activates or Deactivates Cell Balancing according to Status (should be the next tick of @ref MAX17852_GetBalancingStatus
    * @details Starts Reading Process of BALSTAT & BALUVSTAT for later Status Evaluation
    * @param BalCellData pointer to most recent cell measurement data
    * @param BalDCSize Size of Daisy Chain
    * @returns 0 if no errors -1 if error
    */
//char MAX17852_DoBalancing(DeviceData *BalCellData, char BalDCSize);

#endif