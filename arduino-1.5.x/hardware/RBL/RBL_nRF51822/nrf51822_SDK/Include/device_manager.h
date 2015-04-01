/* Copyright (C) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * @file device_manager.h
 *
 * @defgroup device_manager Device Manager
 * @ingroup experimental_api
 * @{
 * @brief Device Manager Application Interface Abstraction..
 *
 * @details Device Manager Active and Bond Peers. Management of peer includes
 *          book keeping of contextual information book keeping like the Security Keys, GATT
 *          configuration and any application specific information.
 *
 *          Active Peers are devices which connected, and may or may not be bonded.
 *          Bonded Peers are devices which bonded, and may or may not be Active (Connected).
 *          Therefore, Active Bonded Peer refers to a device which is connected as well as
 *          bonded.
 *          Paired Devices or refers to peer devices that are connected and have necessary context
 *          establishment/exchange for the current connection session, however, on disconnection
 *          all contextual information is flushed. For example, SMP Information Exchanged during
 *          pairing, GATT Configuration etc is not retained on disconnection.
 *
 *          It should be noted that this module allows management of contextual information but not
 *          does provide an interface for connection management. Therefore, entering connectable
 *          mode, or connection establishment or disconnection of a link with peer is not in scope
 *          of this module.
 *
 *          For bonded peers, the contextual information is required to be retained on disconnection
 *          and on  power cycle, persistent storage of contextual information is handled by the
 *          module. This module categorizes the contextual information into 3 categories:
 *             - <b>Bonding Information</b>
 *               This information is the information exchanged between local and peer device to
 *               establish a bond. This information also includes peer identification information,
 *               like the peer address or the IRK or both. From here on this category of information
 *               is referred to as Device Context.
 *             - <b>Service/Protocol Information</b>
 *               This information is the information retained for the peer to save on one-time
 *               procedures like the GATT Service Discovery procedures, Service Configurations etc,
 *               allowing devices to resume data exchange on subsequent reconnection without having
 *               to perform initial set-up procedures each time. From here on this category is
 *               referred to as Service Context.
 *             - <b>Application Information</b>
 *               This information is the context that the application would like to associate with
 *               each of the bonded device. For example, if application chooses to rank its peers
 *               in order to manage them better the rank information could be treated as
 *               Application Information. This provision is provided to save the application from
 *               maintaining a mapping table with each Device Instance and Application Information.
 *               However, if application have no use case to make use of this, it is possible to not
 *               use or employ this at compile time. From here on this category of information is
 *               referred to as Application Context.
 */


#ifndef DEVICE_MANAGER_H__
#define DEVICE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "ble.h"
#include "ble_gap.h"
#include "device_manager_cnfg.h"

/**
 * @defgroup dm_service_cntext_types Service/Protocol Types
 *
 * @brief Describes the possible types of Service/Protocol Contexts for a bonded/peer device.
 *
 * @details Possible Service/Protocol context per peer device. Device Manager provides the
 *          functionality of not only persistently storing the Service/Protocol context but auto
 *          loading them when needed.
 *          For example system attributes for a GATT Server. However, based on nature of
 *          application, not all service types may be needed. In which case application and specify
 *          only the service/protocol context it is interested in at the time of registration.
 * @{
 */
#define DM_PROTOCOL_CNTXT_NONE                  0x00  /**< No Service Context, this implies the application does not want to associate any service/protocol context with the peer device */
#define DM_PROTOCOL_CNTXT_GATT_SRVR_ID          0x01  /**< GATT Server Service Context, this implies the application does associate GATT Server with the peer device and this information will be auto loaded when needed for a bonded device */
#define DM_PROTOCOL_CNTXT_GATT_CLI_ID           0x02  /**< GATT Client Service Context, this implies the application does associate GATT Client with the peer device and this information will be auto loaded when needed for a bonded device */
#define DM_PROTOCOL_CNTXT_ALL                                                                     \
        (DM_PROTOCOL_CNTXT_GATT_SRVR_ID | DM_PROTOCOL_CNTXT_GATT_CLI_ID) /**< All Service/Protocol Context, this implies that the application wants to associate all Service/Protocol Information with the bonded device. This is configurable based on system requirements, if the application is has only one type of service applications, this define could be altered to reflect the same.  */
/** @} */


/**
 * @defgroup dm_events Device Manager Events
 *
 * @brief This section describes the device manager events to that are notified to the application.
 *
 * @details Device Manager notifies the application of various asynchronous events using the
 *          asynchronous event notification callback. All events has been categorized into:
 *          a. General.
 *          b. Link Status.
 *          c. Context Management.
 *
 *          In the callback, these events are notified along with handle, that uniquely identifies
 *          application instance, active instance (if applicable), device instance
 *          (bonding instance, if applicable) and service instance. Not all events are pertaining to
 *          an active connection, for example a context deletion event could occur even if the peer
 *          is not connected. Also, general category of events may not be pertaining to any specific
 *          peer.
 *          See also \ref dm_event_notification_cb and \ref dm_application_register.
 * @{
 */
/**
 * @defgroup general_events General Events
 *
 * @brief General or miscellaneous events.
 *
 * @details This category of events are general events not pertaining to a peer or context.
 *
 * @{
 */
#define DM_EVT_RFU                          0x00    /**< Reserved for future use, is never notified */
#define DM_EVT_ERROR                        0x01    /**< Indicates link with peer device is established. */
#define DM_EVT_ACTIVE_DEVICE_DB_FULL        0x02    /**< Indicates no more active devices can be managed by the module. */
#define DM_EVT_BONDED_DEVICE_DB_FULL        0x03    /**< Indicates no more bonded devices contexts can be managed by the module. */
/** @} */

/**
 * @defgroup link_status_events Link Status Events
 *
 * @brief Link Status Events.
 *
 * @details This category of events notify the application of link status. Event result associated
 *          with the event provided along with the event in the callback provide more details of
 *          whether a procedure succeeded or failed and assist the application in decision making of
 *          how to proceed. For example if a DM_DEVICE_CONNECT_IND is indicated with NRF_SUCCESS
 *          result, the application may want to proceed with discovering and association with
 *          service of the peer, however, if indicated with a failure result, the application may
 *          want to take alternate action getting into connectable mode again or getting into a
 *          sleep mode etc.
 *
 * @{
 */
#define DM_EVT_CONNECTION               0x11  /**< Indicates link with peer is established. */
#define DM_EVT_DISCONNECTION            0x12  /**< Indicates link with peer is torn down. */
#define DM_EVT_SECURITY_SETUP           0x13  /**< Security procedure for link started indication */
#define DM_EVT_SECURITY_SETUP_COMPLETE  0x14  /**< Security procedure for link completion indication. */
#define DM_EVT_LINK_SECURED             0x15  /**< Indicates link with peer is secured. For bonded devices, subsequent reconnections with bonded peer will result only in this event when the link is secured and setup procedures will not occur unless the bonding information is either lost or deleted on either or both sides.  */
#define DM_EVT_SECURITY_SETUP_REFRESH   0x16  /**< Indicates security on the link was refreshed/ re-established. */
/** @} */

/**
 * @defgroup context_mgmt_events Context Management Events
 *
 * @brief Context Management Events.
 *
 * @details These events notify application of status loading or storing of contexts.
 *
 * @{
 */
#define DM_EVT_DEVICE_CONTEXT_LOADED    0x21 /**< Indicates device context for a peer was loaded. */
#define DM_EVT_DEVICE_CONTEXT_STORED    0x22 /**< Indicates device context is stored persistently. */
#define DM_EVT_DEVICE_CONTEXT_DELETED   0x23 /**< Indicates device context is deleted. */
#define DM_EVT_SERVICE_CONTEXT_LOADED   0x31 /**< Indicate service context for a peer is loaded. */
#define DM_EVT_SERVICE_CONTEXT_STORED   0x32 /**< Indicates service context is stored persistently. */
#define DM_EVT_SERVICE_CONTEXT_DELETED  0x33 /**< Indicates service context is deleted. */
#define DM_EVT_APPL_CONTEXT_LOADED      0x41 /**< Indicate application context for a peer is loaded. */
#define DM_EVT_APPL_CONTEXT_STORED      0x42 /**< Indicates application context is stored persistently. */
#define DM_EVT_APPL_CONTEXT_DELETED     0x43 /**< Indicates application context is deleted. */
/** @} */
/** @} */

/**
 * @defgroup dm_data_structure Device Manager Data Types
 *
 * @breif This section describes all the data types exposed by the module to the application.
 * @{
 */

/**
 * @brief Application Instance.
 *
 * @details Application instance uniquely identifies an application. The identifier is allocated by
 *          the device manager when application registers with the module. The application is
 *          expected to identify itself with this instance identifier when initiating subsequent
 *          requests. Application should use the utility API \ref dm_application_instance_set in
 *          order to set its application instance in dm_handle_t needed for all subsequent APIs.
 *          See also \ref dm_application_register.
 */
typedef uint8_t  dm_application_instance_t;

/**
 * @brief Connection Instance.
 *
 * @details Identifies connection instance for an active device. This instance is allocated by the'
 *          device manager when a connection is established which is notified with DM_CONNECTION_IND
*           with event result NRF_SUCCESS.
 */
typedef uint8_t dm_connection_instance_t;


/**
 * @brief Device Instance.
 *
 * @details Uniquely identifies a peer entity. The peer entity may or may not be connected. This
 *          instance is allocated by the device manager when a device is bonded and this notified
 *          when security procedures have been initiated notified with DM_SECURITY_SETUP_IND with
 *          success event result. In case the event result indicates that the module cannot add more
 *          bonded devices, no instance is allocated. Application can chose to disconnect the link.
 */
typedef uint8_t  dm_device_instance_t;

/**
 * @brief Device Instance.
 *
 * @details Uniquely identifies a peer entity. The peer entity may or may not be connected. This
 *          instance is allocated by the device manager when a device is bonded and this notified
 *          when security procedures have been initiated notified with DM_SECURITY_SETUP_IND with
 *          success event result. In case the event result indicates that the module cannot add more
 *          bonded devices, no instance is allocated. Application can chose to disconnect the link.
 */
typedef uint8_t  dm_service_instance_t;

/**
 * @brief Service/Protocol Type Identifier.
 *
 * @details Uniquely identifies a service or a protocol type. Service/Protocol Type identification
 *          is needed as each service/protocol can have its own contextual data and there can be
 *          more than one service accessed by the peer. \ref dm_service_cntext_types describes the
 *          list of services/protocols supported.
 */
typedef uint8_t  service_type_t;

/**
 * @brief Device Handle used for unique identification of each peer.
 *
 * @details This data type is used to uniquely identify each peer device. A peer device could be
 *          active and/or bonded. Therefore an instance for active and bonded is provided.
 *          Application is however expected to treat this is an opaque structure and use this for
 *          all API interactions once stored on appropriate events.
 *          See \ref dm_events.
 */
typedef struct device_handle
{
    dm_application_instance_t    appl_id;       /**< Identifies the application instances for which the device is being managed. */
    dm_connection_instance_t     connection_id; /**< Identifies connection instance. Active connection instance. */
    dm_device_instance_t         device_id;     /**< Identifies peer instance in the data base. */
    dm_service_instance_t        service_id;    /**< Service instance identifier. */
}dm_handle_t;

/**
 * @brief Context Data Definition
 *
 * @details Defines contextual data format, it consists of context data length and pointer to data.
 */
typedef struct
{
    uint32_t    len;        /**< Length of data . */
    uint8_t    * p_data;   /**< Pointer to contextual data, a copy is made of the data. */
}dm_context_t;

typedef dm_context_t dm_device_context_t;

/**
 * @brief Service Context.
 *
 * @details Service Context Data for a service identified by 'service_type' field.
 */
typedef struct
{
    service_type_t    service_type;      /**< Identifies the service/protocol to which the context data is related. */
    dm_context_t      context_data;      /**< Contains length and pointer to context data */
}dm_service_context_t;


/**
 * @brief Application context.
 *
 * @details The application context can be used by the application to map any application level
 *          information that is to be mapped with a particular peer.
 *          For bonded peers, this information will be stored by the bond manager persistently.
 *          However, it should be noted that the device manager treats this information as an
 *          opaque blob.
 *          Necessary APIs to get and set this context for a peer have been provided
 */
typedef dm_context_t dm_application_context_t;

/**
 * @brief Event parameters.
 *
 * @details Defines event parameters for each of the events notified by the module.
 */
typedef union
{
    ble_gap_evt_t               * p_gap_param;       /**< All events that are triggered in device manager as a result of GAP events, like connection, disconnection, security procedures are accompanied with GAP parameters. */
    dm_application_context_t    * p_app_context;     /**< All events that are associated with application context procedures of store, load, and deletion have this as event parameter. */
    dm_service_context_t        * p_service_context; /**< All events that are associated with service context procedures of store, load and deletion have this as event parameter. */
    dm_device_context_t         * p_device_context;  /**< All events that are associated with device context procedures of store, load and deletion have this as event parameter. */
}dm_event_param_t;

/**
 * @brief Asynchronous events details notified to the application by the module.
 *
 * @details Defines event type along with event parameters notified to the application by the
 *          module.
 */
typedef struct
{
    uint8_t             event_id;          /**< Identifies the event. See \ref conn_mngr_events for details on event types and their significance. */
    dm_event_param_t    event_param;   /**< Event parameters, can be null if the event does not have any parameters. */
    uint16_t            event_paramlen;   /**< Length of event parameters, is zero if event does not have any parameters. */
} dm_event_t;

/**
 * @brief Event notification callback registered by application with the module.
 *
 * @details Event notification callback registered by application with the module when registering
 *          the module using \ref dm_application_register API.
 *
 * @param p_handle      Identifies the peer for which the event is being notified.
 * @param p_event       Identifies the event, any associated parameters and parameter length.
 *                      See \ref dm_events for details on event types and their significance.
 * @param event_result  Provide additional information on the event. For example, in case of error
 *                      event, one of the SDK error codes is used to indicate nature of error.
 *                      Additionally, indications of data base being maximum connection instances
 *                      that can be managed being reached is indicated with connection or bonding
 *                      events.
 *
 * @return Application can return a success or a failure to indicate if it could handle the event
 *         successfully. There is no action taken in case application returns a failure.
 */
typedef api_result_t (*dm_event_cb_t)(dm_handle_t const    * p_handle,
                                      dm_event_t const     * p_event,
                                      api_result_t           event_result);

/**
 * @brief Initialization Parameters.
 *
 * @details Indicates application parameters. Currently only takes information of clearing
 *          all persistent data.
 */
typedef struct
{
    bool clear_persistent_data; /**< Set to true in case the module should clear all persistent data */
}dm_init_param_t;

/**
 * @brief Application Registration Parameters.
 *
 * @details Parameters that application furnishes to the module when registering with it.
 */
typedef struct
{
    dm_event_cb_t          evt_handler;  /**< Event Handler to be registered to receive asynchronous notification from the module, see \ref dm_events for asynchronous events */
    uint8_t                service_type; /**< Bit mask identifying services that the application intends to support for all peers. */
    ble_gap_sec_params_t   sec_param;    /** Security parameters to be used for the application. */
}dm_application_param_t;

/**
 * @brief Defines possible states security status.
 *
 * @details Defines possible states security status of a link when requested by application using
 *          the \ref dm_security_status_req.
 */
typedef enum{
    NONE,
    PROCEDURE_IN_PROGRESS,
    ENCRYPTED
}dm_security_status_t;
/** @} */


/**
 * @defgroup dm_api Device Module APIs
 *
 * @brief This section describes APIs exposed by the module.
 *
 * @details This section describes APIs exposed by the module; APIs have been categorized to provide
 &          better and specific look up for developers. Categories are:
 *          a. Set up APIs.
 *          b. Context Management APIs.
 *          c. Utility APIs.
 *
 *          MSCs describe usage of these APIs. See @mscfile dm_api.dox
 * @{
 */
/**
 * @defgroup dm_setup_api Device Module Set-up APIs
 *
 * @brief Initialization & registration APIs that are pre-requisite for all other module procedures.
 * @details This section describes Module Initialization & Registration APIs needed to be set-up by
 *          the application before device manager can start managing devices and device contexts
 *          for the application.
 *
 * @{
 */


/**
 * @brief Module Initialization Routine.
 *
 * @details Initializes module. To be called once before any other APIs of the module are used.
 *
 * @param[in] Initialization parameters.
 *
 * @return NRF_SUCCESS or appropriate error code, indicating reason for failure.
 */
api_result_t dm_init(dm_init_param_t   const * p_init_param);


/**
 * @brief Application registration routine.
 *
 * @details This routine is used by the application to register for asynchronous events with the
 *          device manager. The application, during registration also indicates the services that it
 *          intends to support on this instance. It is possible to register multiple times with the
 *          device manager. At least one instance  shall be registered with the device manager after
 *          module has been initialized.
 *          Maximum number of application instances device manager can support is determined
 *          DM_MAX_APPLICATIONS.
 *
 *          All applications shall be registered before initiating or accepting connections from
 *          peer is a must.
 *
 * @param[out] p_appl_instance   Application Instance Identifier in case registration is successful.
 * @param[in]  p_appl_param      Application parameters.
 *
 * @return
 */
api_result_t dm_register(dm_application_instance_t       * p_appl_instance,
                         dm_application_param_t    const * p_appl_param);


/**
 * @brief BLE Event Handler.
 * @details This routine should be called from BLE stack event dispatcher for the module to function
 *          as expected.
 * @param[in] p_ble_evt BLE stack event being dispatched to the function.
 *
 * @return None.
 */
void dm_ble_evt_handler(ble_evt_t  * p_ble_evt);

/** @} */


/**
 * @defgroup dm_security_api APIs to set-up or know status of security on a link.
 *
 * @brief This section describes APIs to Set Up Security. These APIs require that the peer be
 *        connected for the procedures to requested.
 *
 * @details This group allows application to request security procedures or know status of security on a link.
 * @{
 */
/**
 * @brief Routine to request setting up security on a link.
 *
 * @details This API initiates security procedures with a peer device. DM_SECURITY_SETUP_IND
 *          event is notified to the application when the procedure is complete.
 *          @note In case of GAP Central role, in case peer is not bonded, request to bond is
 *          initiated, else, link is re-encrypted using the existing bond information. In case of
 *          GAP peripheral role, a Slave security request is sent.
 *
 * @param[in] p_handle   Identifies the link on which security is desired.
 *
 * @return \ref NRF_SUCCESS or appropriate error code indicating reason for failure.
 */
api_result_t dm_security_setup_req(dm_handle_t * p_handle);


/**
 * @brief Routine to know status of security on a link.
 *
 * @details This API allows application to query status of security on a link.
 *
 * @param[in] p_handle  Identifies the link on which security is desired.
 * @param[out] p_status Pointer where security status is provided to the application.
 *
 * @return \ref NRF_SUCCESS or appropriate error code indicating reason for failure.
 */
api_result_t dm_security_status_req(dm_handle_t const * p_handle, dm_security_status_t * p_status);


/**
 * @brief Routine to create white-list.
 *
 * @details This API allows application to create whitelist based on bonded peer devices in module
 *          data base.
 *
 * @param[in] p_handle  Identifies the application requesting white-list creation.
 * @param[out] p_whitelist Pointer where created white-list is provided to the application.
 *
 * @note 'addr_count' and 'irk_count' fields of the structure should be populated with maximum
 *       number of devices that the application wishes to request in the whitelist. In case, number
 *       of bonded device list are less than requested, these fields will be updated with actuals.
 *       In case number of devices are more than numbers requested, module will populate the list
 *       with devices in order of bond established with the peer devices. Also, if this routine is
 *       is called when a connection exists with one or more peer devices, device(s) with which
 *       connection exists are not added to the whitelist.
 *
 * @return \ref NRF_SUCCESS or appropriate error code indicating reason for failure.
 */
api_result_t dm_whitelist_create(dm_application_instance_t const * p_handle,
                                 ble_gap_whitelist_t             * p_whitelist);

/** @} */


/**
 * @defgroup dm_cntxt_mgmt_api Context Management APIs
 *
 * @brief Utility APIs offered by the device manager to get information about the peer if and
 *        when needed.
 *
 * @details This group of API allows application to access information that is not required to be
 *          maintained by the application but may be needed. Hence it is possible to get the
 *          information from the module is instead of mapping all the information with a device
 *          context by itself.
 * @{
 */
/**
 * @brief Add peer device context and all related information to database.
 *
 * @param[out] p_handle  Contains device identification of the new device added.
 * @param[in]  p_context Contextual information added for the device.
 *
 * @note This API is envisioned to be used for adding device from out of band mechanism to the
 *       data base maintained by the module. Information added as a part of context shall include
 *       peer identification and bonding information.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_device_add(dm_handle_t                  * p_handle,
                           dm_device_context_t const    * p_context);


/**
 * @brief Delete peer device context and all related information from database.
 *
 * @param[in] p_handle Identifies the peer device to be deleted.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_device_delete(dm_handle_t const * p_handle);


/**
 * @brief Delete all peer device context and all related information from database.
 *
 * @param[in] p_handle Identifies application instance that is requesting
 *                     delete of all bonded devices.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_device_delete_all(dm_application_instance_t const * p_handle);


/**
 * @brief Set Service Context for a peer device identified by 'p_handle' parameter.
 *
 * @details This API allows application to Set Service Context for a peer device identified by
 *          'p_handle' parameter. This API is useful when Service Context cannot be requested
 *          from SoftDevice, but needs to assembled by the application or an another module or when
 *          service context is exchanged in an out of band way.
 *          This API could also be used to trigger a storing of service context into persistent 
 *          memory. If this is desired, a NULL pointer could be passed to the p_context.
 *
 * @param[in] p_handle Identifies peer device for which procedure is requested.
 *
 * @param[in] p_context Service context being set. The context information includes length of
 *                      data and pointer to the contextual data being set. The memory pointed to by
 *                      the pointer to data is assumed to be resident when API is being called and
 *                      can be freed or reused once the set procedure is complete. Set procedure
 *                      completion is indicated by the event \ref DM_EVT_SERVICE_CONTEXT_STORED.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_service_context_set(dm_handle_t const           * p_handle,
                                    dm_service_context_t const  * p_context);


/**
 * @brief Get Application Context for a peer device identified by 'p_handle' parameter.
 *
 * @param[in] p_handle Identifies peer device for which procedure is requested.
 *
 * @param[in] p_context Application context being requested. The context information includes length
 *            of data and pointer to the contextual data is provided. Note that requesting a 'get'
 *            of application does not need to provide memory, the pointer to data will be rightly
 *            pointing to application data and hence no data movement is involved.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_service_context_get(dm_handle_t const      * p_handle,
                                    dm_service_context_t   * p_context);


/**
 * @brief Delete Service Context identified for a peer device identified by 'p_handle' parameter.
 *
 * @details This API allows application to delete Service Context identified for a peer device
 *          identified by 'p_handle' parameter.
 *
 * @param[in] p_handle  Identifies peer device for which procedure is requested.
 *
 * @param[in] service_type Identifies the service whose context is being deleted.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_service_context_delete(dm_handle_t const    * p_handle);


/**
 * @brief Set Application Context for a peer device identified by 'p_handle' parameter.
 *
 * @details This application allows setting application context for the peer device identified by
 *          'p_handle'. Application context is stored persistently by the module and can be
 *          requested by the application at any time using the \ref dm_application_context_get
 *          API. It should be noted that this procedure is permitted only for bonded devices. If the
 *          device is not bonded, application context cannot be set. However, it is not mandatory
 *          that the bonded device is connected when requesting this procedure.
 *
 * @param[in] p_handle  Identifies peer device for which procedure is requested.
 *
 * @param[in] p_context Application context being set. The context information includes length of
 *                      data and pointer to the contextual data being set. The memory pointed to by
 *                      the pointer to data is assumed to be resident when API is being called and
 *                      can be freed or reused once the set procedure is complete. Set procedure
 *                      completion is indicated by the event \ref DM_EVT_APPL_CONTEXT_STORED.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_application_context_set(dm_handle_t const                 * p_handle,
                                        dm_application_context_t const    * p_context);



/**
 * @brief Get Application Context for a peer device identified by 'p_handle' parameter.
 *
 * @details Get Application Context for a peer device identified by 'p_handle' parameter.
 *
 * @param[in] p_handle Identifies peer device for which procedure is requested.
 *
 * @param[in] p_context Application context being requested. The context information includes length
 *                      of data and pointer to the contextual data is provided.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_application_context_get(dm_handle_t const           * p_handle,
                                        dm_application_context_t    * p_context);


/**
 * @brief Delete Application Context for a peer device identified by 'p_handle' parameter.
 *
 * @param[in] p_handle Identifies peer device for which procedure is requested.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_application_context_delete(dm_handle_t const   * p_handle);

/** @} */


/**
 * @defgroup utility_api Utility APIs
 * @{
 * @brief This section describes the utility APIs offered by the module.
 *
 * @details APIs defined in this section are utility or assisting/helper APIs.
 */
/**
 * @brief Set/Copy Application instance to Device Manager handle.
 *
 * @param[in]  p_appl_instance Application instance to be set.
 * @param[out] p_handle        Device Manager handle for which the instance is to be copied.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_application_instance_set(dm_application_instance_t const    * p_appl_instance,
                                         dm_handle_t const                  * p_handle);
                                         
/**
 * @brief Get peer's device address.
 *
 * @param[in]  p_handle Identifies the peer device whose address is requested.
 * @param[out] p_addr   Pointer where address is to be copied.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_peer_addr_get(dm_handle_t const    * p_handle,
                              ble_gap_addr_t       * p_addr);


/**
 * @brief Set/Update peer's device address.
 *
 * @param[in]  p_handle Identifies the peer device whose address is requested to be set/updated.
 * @param[out] p_addr   Address to be set/updated with.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_peer_addr_set(dm_handle_t const      * p_handle,
                              ble_gap_addr_t const   * p_addr);


/**
 * @brief Initialize Device Manager handle.
 *
 * @param[in] p_handle Device Manager handle to be initialized.
 *
 * @return NRF_SUCCESS or an appropriate error code indicating reason for failure.
 */
api_result_t dm_handle_initialize(dm_handle_t    * p_handle);
/** @} */
/** @} */
/** @} */
#ifdef __cplusplus
}
#endif
#endif // DEVICE_MANAGER_H__

