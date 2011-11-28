/* 
 * File:   IPanel.h
 * Author: marok
 *
 * Created on 28 listopad 2011, 02:00
 */

#ifndef IPANEL_H
#define	IPANEL_H
class IPanel
{
public:
	virtual void updatePanel() = 0;
	virtual GtkWidget* createPanel() = 0;
};


#endif	/* IPANEL_H */

